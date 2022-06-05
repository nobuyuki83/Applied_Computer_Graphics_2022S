#include <vector>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include "gltf_util.h"
#include <Eigen/Geometry>

#include "delfem2/glfw/viewer3.h"
#include "delfem2/glfw/util.h"
#include "delfem2/opengl/old/funcs.h"
#include "delfem2/opengl/old/mshuni.h"

class ChannelData {
 public:
  unsigned int ibone;
  std::vector<double> v_input, v_output;
  std::string target_path;
};

/**
 *@brief articulated rigid body for character rigging
 */
class RigBone {
 public:
  RigBone()
      : inverseBindMatrix{}, transformGlobal{} {
    parent_bone_idx = -1;
    transformRelative.setIdentity();
    inverseBindMatrix.setIdentity();
    transformGlobal.setIdentity();
  }

 public:
  std::string name; // name of this bone

  /**
   * @brief Inverse of the binding matrix.
   * @details The binding matrix transform the bone to its reference config.
   * With binding matrix, the origin is mapped to the joint position of this bone in the reference config
   */
  Eigen::Matrix4d inverseBindMatrix;

  /**
   * @brief index of parent bone
   */
  unsigned int parent_bone_idx;

  /**
   * @brief transformation of current bone relative to parent bone
   */
  Eigen::Matrix4d transformRelative;

  /**
   * @brief Affine matrix to transform the bone to the current config
   */
  Eigen::Matrix4d transformGlobal;
};

void LoadBoneFromGltfFile(
    std::vector<RigBone> &bones,
    const tinygltf::Model &model,
    unsigned int inode0,
    int ibone_p,
    const std::vector<unsigned int> &mapNode2Bone) {
  assert(inode0 < model.nodes.size());
  const tinygltf::Node &node = model.nodes[inode0];
  const unsigned int ibone0 = mapNode2Bone[inode0];
  if (ibone0 == UINT_MAX) { return; }
  assert(ibone0 < bones.size());
  bones[ibone0].parent_bone_idx = ibone_p;
  bones[ibone0].name = node.name;
  for (int inode_ch: node.children) {
    LoadBoneFromGltfFile(
        bones,
        model, inode_ch, ibone0, mapNode2Bone);
  }
}

bool LoadGltf(
    std::vector<double> &vtx_xyz_ini,
    std::vector<unsigned int> &tri_vtz,
    std::vector<double> &skinning_sparse_weight,
    std::vector<unsigned int> &skinning_sparse_index,
    std::vector<RigBone> &bones,
    std::vector<ChannelData> &channels,
    unsigned int iskin,
    int imesh,
    int iprimitive,
    int ianimation,
    const std::string &fpath) {
  std::string err;
  std::string warn;
  tinygltf::TinyGLTF loader;
  std::unique_ptr<tinygltf::Model> pModel = std::make_unique<tinygltf::Model>();
  bool ret = loader.LoadASCIIFromFile(
      pModel.get(), &err, &warn,
      fpath);
  if (!warn.empty()) { printf("Warn: %s\n", warn.c_str()); }
  if (!err.empty()) { printf("Err: %s\n", err.c_str()); }
  if (!ret) {
    printf("Failed to parse glTF\n");
    return false;
  }
  tinygltf::Model &model = *(pModel);
  //
  LoadMeshInfo(
      vtx_xyz_ini, tri_vtz,
      skinning_sparse_weight, skinning_sparse_index,
      model, imesh, iprimitive);
  //
  assert(iskin < model.skins.size());
  bones.resize(model.skins[iskin].joints.size());
  unsigned int inode_root = model.skins[iskin].skeleton;
  if (inode_root == UINT_MAX && !model.skins[iskin].joints.empty()) {
    inode_root = model.skins[iskin].joints[0];
  }
  assert(inode_root < model.nodes.size());
  //
  std::vector<unsigned int> mapNode2Bone(model.nodes.size(), UINT_MAX);
  for (unsigned int ij = 0; ij < model.skins[iskin].joints.size(); ++ij) {
    const unsigned int inode = model.skins[iskin].joints[ij];
    assert(inode < model.nodes.size());
    mapNode2Bone[inode] = ij;
  }
  LoadBoneFromGltfFile(
      bones,
      model, inode_root, -1, mapNode2Bone);
  { // set bone invBindMat
    const tinygltf::Skin &skin = model.skins[iskin];
    std::vector<double> M;
    GetArray_Double(M, model, skin.inverseBindMatrices);
    assert(M.size() == bones.size() * 16);
    for (size_t ij = 0; ij < M.size() / 16; ++ij) {
      for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
          bones[ij].inverseBindMatrix(i, j) = M[ij * 16 + j * 4 + i];
        }
      }
      bones[ij].transformGlobal = bones[ij].inverseBindMatrix.inverse().eval();
    }
  }
  { // load bone animation
    std::vector<tinygltf::AnimationSampler> &sampler = pModel->animations[ianimation].samplers;
    std::vector<tinygltf::AnimationChannel> &channel = pModel->animations[ianimation].channels;
    const unsigned int nch = channel.size();
    channels.resize(nch);
    for (unsigned int ich = 0; ich < channel.size(); ++ich) {
      channels[ich].target_path = channel[ich].target_path;
      const int isampler = channel[ich].sampler;
      const int inode = channel[ich].target_node;
      channels[ich].ibone = mapNode2Bone[inode];
      assert(channels[ich].ibone < bones.size());
      GetArray_Double(channels[ich].v_input, model, sampler[isampler].input);
      GetArray_Double(channels[ich].v_output, model, sampler[isampler].output);
    }
  }
  return true;
}

/**
 * set relative transformation to each bone
 */
void SetRelativeTransformationToBones(
    std::vector<RigBone> &bones,
    std::vector<ChannelData> &channels,
    double time) {
  for (auto &bone: bones) {
    bone.transformRelative.setIdentity();
  }
  for (auto &ch: channels) {
    unsigned int idx1 = 0;
    for (; idx1 < ch.v_input.size(); ++idx1) {
      if (ch.v_input[idx1] > time) { break; }
    }
    idx1 = (idx1 == ch.v_input.size()) ? idx1 - 1 : idx1;
    unsigned int idx0 = (idx1 == 0) ? 0 : idx1 - 1;
    double ratio = (idx1 == idx0) ? 0 : (time - ch.v_input[idx0]) / (ch.v_input[idx1] - ch.v_input[idx0]);
    if (ch.target_path == "translation") {  // translation
      const Eigen::Translation3d transl(
          (1 - ratio) * ch.v_output[idx0 * 3 + 0] + ratio * ch.v_output[idx1 * 3 + 0],
          (1 - ratio) * ch.v_output[idx0 * 3 + 1] + ratio * ch.v_output[idx1 * 3 + 1],
          (1 - ratio) * ch.v_output[idx0 * 3 + 2] + ratio * ch.v_output[idx1 * 3 + 2]);
      bones[ch.ibone].transformRelative *= Eigen::Affine3d(transl).matrix();
    }
    if (ch.target_path == "rotation") {  // rotation in quaternion
      const Eigen::Quaterniond quat(
          (1 - ratio) * ch.v_output[idx0 * 4 + 3] + ratio * ch.v_output[idx1 * 4 + 3],
          (1 - ratio) * ch.v_output[idx0 * 4 + 0] + ratio * ch.v_output[idx1 * 4 + 0],
          (1 - ratio) * ch.v_output[idx0 * 4 + 1] + ratio * ch.v_output[idx1 * 4 + 1],
          (1 - ratio) * ch.v_output[idx0 * 4 + 2] + ratio * ch.v_output[idx1 * 4 + 2]);
      bones[ch.ibone].transformRelative *= Eigen::Affine3d(quat).matrix();
    }
  }
}

void LinearBlendSkinning(
    std::vector<RigBone> &bones,
    std::vector<double> &vtx_xyz,
    const std::vector<double> &vtx_xyz_ini,
    const std::vector<double> &skinning_sparse_weight,
    const std::vector<unsigned int> &skinning_sparse_index) {
  // set global transformation using the relative transformation for each bone
  for (std::size_t ibone = 0; ibone < bones.size(); ++ibone) {
    const unsigned int ibone_p = bones[ibone].parent_bone_idx;
    Eigen::Matrix4d transformParent = Eigen::Matrix4d::Identity();
    if (ibone_p != UINT_MAX) { // this is not root bone
      transformParent = bones[ibone_p].transformGlobal;  // set parent bone transformation
    }
    // bones[ibone].transformGlobal = /* write single line code here */
  }
  // deform vertex positions of the triangle mesh using linear blend skinning
  const size_t nvtx = vtx_xyz.size() / 3;
  for (unsigned int ip = 0; ip < nvtx; ++ip) { // loop over vertex
    Eigen::Vector4d pos0( // initial position (red)
        vtx_xyz_ini[ip * 3 + 0],
        vtx_xyz_ini[ip * 3 + 1],
        vtx_xyz_ini[ip * 3 + 2], 1.0);
    Eigen::Vector4d pos1(0, 0, 0, 1);  // deformed position (blue)
    double sum_w = 0.0;
    for (int iibone = 0; iibone < 4; ++iibone) {  // each vertex has weights of 4 bones
      double w = skinning_sparse_weight[ip * 4 + iibone];  // rigging weight
      if (w < 1.0e-30) { continue; }
      unsigned int ibone = skinning_sparse_index[ip * 4 + iibone];  // bone index
      assert (ibone < bones.size());
      Eigen::Matrix4d inverseBindMatrix = bones[ibone].inverseBindMatrix;
      Eigen::Matrix4d transformGlobal = bones[ibone].transformGlobal;
      sum_w += w;
      // pos1 = /* write single line code here */
    }
    assert(fabs(sum_w) > 1.0e-10);
    pos1 /= sum_w;
    vtx_xyz[ip * 3 + 0] = pos1[0];
    vtx_xyz[ip * 3 + 1] = pos1[1];
    vtx_xyz[ip * 3 + 2] = pos1[2];
  }
}

// ---------------------------------

int main() {
  std::vector<double> vtx_xyz;  // deformed vertex positions (blue)
  std::vector<unsigned int> tri_vtx;  // triangle index
  std::vector<double> skinning_sparse_weight;  // skinning weights for vertex
  std::vector<unsigned int> skinning_sparse_index;  // bones for vertex
  std::vector<RigBone> bones;
  std::vector<ChannelData> channels;
  LoadGltf(vtx_xyz, tri_vtx,
           skinning_sparse_weight, skinning_sparse_index,
           bones, channels,
           0, 0, 0, 0,
           std::string(SOURCE_DIR) + "/../assets/CesiumMan.gltf");
  const std::vector<double> vtx_xyz_ini = vtx_xyz;  // initial vertex position (red)

  // --------------
  // opengl starts here
  delfem2::glfw::CViewer3 viewer(2);
  viewer.window_title = "task06";
  viewer.view_rotation = std::make_unique<delfem2::ModelView_Ztop>();
  if (!glfwInit()) { exit(EXIT_FAILURE); }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  viewer.OpenWindow();
  while (!glfwWindowShouldClose(viewer.window)) {
    double time = glfwGetTime();
    time = time - 2. * int(time / 2.);  // loop with 2 sec interval
    // for each bone set relative rotation from parent bone
    SetRelativeTransformationToBones(
        bones, channels,
        time);
    // linear blend skinning
    LinearBlendSkinning(
        bones, vtx_xyz,
        vtx_xyz_ini,
        skinning_sparse_weight, skinning_sparse_index);
    // --------------------
    viewer.DrawBegin_oldGL();
    ::glDisable(GL_LIGHTING);
    delfem2::opengl::DrawAxis(1);
    // draw reference
    glColor3d(1, 0, 0);
    delfem2::opengl::DrawMeshTri3D_Edge(
        vtx_xyz_ini.data(), vtx_xyz_ini.size() / 3,
        tri_vtx.data(), tri_vtx.size() / 3);
    glColor3d(0, 0, 1);
    delfem2::opengl::DrawMeshTri3D_Edge(
        vtx_xyz.data(), vtx_xyz.size() / 3,
        tri_vtx.data(), tri_vtx.size() / 3);
    viewer.SwapBuffers();
    glfwPollEvents();
  }
  glfwDestroyWindow(viewer.window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}


