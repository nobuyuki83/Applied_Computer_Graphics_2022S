/*
 * Copyright (c) 2019 Nobuyuki Umetani
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <vector>
#include <algorithm>
#if defined(_WIN32) // windows
#  define NOMINMAX   // to remove min,max macro
#  include <windows.h>  // this should come before glfw3.h
#endif
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include "delfem2/srch_trimesh3_class.h"
#include "delfem2/srch_bruteforce.h"
#include "delfem2/srch_bv3_sphere.h"
#include "delfem2/srch_bvh.h"
#include "delfem2/msh_affine_transformation.h"
#include "delfem2/msh_io_ply.h"
#include "delfem2/msh_normal.h"
#include "delfem2/mat4.h"
#include "delfem2/sampling.h"
#include "delfem2/opengl/tex.h"
#include "delfem2/glfw/viewer3.h"
#include "delfem2/glfw/util.h"

namespace dfm2 = delfem2;

// ----------------------------------------

/*! function to compute "weight" and "direction"
 * @param[out] dir
 * @param Xi random number generator
 * @param nrm normal of surface
 * @return weight
 */
double SamplingHemisphere(
    double dir[3],
    std::array<unsigned short, 3> &Xi,
    const double nrm[3])  //
{
  // below implement code to sample hemisphere with cosine weighted probabilistic distribution
  // hint1: use polar coordinate (longitude and latitude).
  // hint2: generate two float values using "dfm2::MyERand48<double>(Xi)". One will be longitude and another will be latitude
  // hint3: for longitude use inverse sampling method to achieve cosine weighted sample.
  // hint4: first assume z is the up in the polar coordinate, then rotate the sampled direction such that "z" will be up.
  // write some codes below (5-10 lines)

  // below: naive implementation to "uniformly" sample hemisphere using "rejection sampling"
  // to not be used for the "problem2" in the assignment
  for(int i=0;i<10;++i) { // 10 is a magic number
    const auto d0 = dfm2::MyERand48<double>(Xi);  // you can sample uniform distribution [0,1] with this function
    const auto d1 = dfm2::MyERand48<double>(Xi);
    const auto d2 = dfm2::MyERand48<double>(Xi);
    dir[0] = d0 * 2 - 1; // dir[0] -> [-1,+1]
    dir[1] = d1 * 2 - 1;
    dir[2] = d2 * 2 - 1;
    double len = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
    if( len > 1 ){ continue; } // reject if outside the unit sphere
    if( len < 1.0e-5 ){ continue; }
    // project on the surface of the unit sphere
    dir[0] /= len;
    dir[1] /= len;
    dir[2] /= len;
    double cos = nrm[0]*dir[0] + nrm[1]*dir[1] + nrm[2]*dir[2]; // cosine weight
    if( cos < 0 ){ continue; }
    return cos*2;  // (coefficient=1/M_PI) * (area_of_hemisphere=M_PI*2) = 2
  }
  return 0;
}

double SampleAmbientOcclusion(
    std::array<unsigned short, 3> &Xi,
    const dfm2::CVec3d &src1,
    const dfm2::CVec3d &dir1,
    const std::vector<double> &vec_xyz,
    const std::vector<unsigned int> &vec_tri,
    const std::vector<dfm2::CNodeBVH2> &bvh_nodes,
    const std::vector<dfm2::CBV3_Sphere<double>> &bvh_volumes) {
  dfm2::PointOnSurfaceMesh<double> pos_mesh;
  bool is_hit = Intersection_Ray3_Tri3_Bvh(
      pos_mesh,
      src1, dir1, vec_xyz, vec_tri, bvh_nodes, bvh_volumes);
  if (!is_hit) { return 0; } // the ray from pixel doesn't hit the mesh
  // ---------------
  unsigned int itri = pos_mesh.itri;  // the triangle hit by the ray
  assert(itri < vec_tri.size() / 3);
  dfm2::CVec3d nrm_tri = dfm2::Normal_TriInMeshTri3(itri, vec_xyz.data(), vec_tri.data());
  nrm_tri.normalize();
  dfm2::CVec3d src2 = pos_mesh.PositionOnMeshTri3(vec_xyz, vec_tri);
  src2 += nrm_tri * 1.0e-3;
  dfm2::CVec3d dir2;
  const double weight0 = ::SamplingHemisphere(
      dir2.p,
      Xi,
      nrm_tri.normalized().data());
  // check if the ray from the triangle hit the mesh
  dfm2::PointOnSurfaceMesh<double> pos_mesh2;
  bool is_hit2 = Intersection_Ray3_Tri3_Bvh(  // check if ray (src2,dir2) hit the triangle mesh
      pos_mesh2,
      src2, dir2, vec_xyz, vec_tri, bvh_nodes, bvh_volumes);
  if (!is_hit2) { return weight0; }
  return 0;
}

int main() {
  std::vector<double> vtx_xyz; // 3d points
  std::vector<unsigned int> tri_vtx;
  { // load input mesh
    delfem2::Read_Ply(
        vtx_xyz, tri_vtx,
        std::string(PATH_SRC_DIR) + "/../assets/bunny_2k.ply");
    dfm2::Rotate_Points3(vtx_xyz, -M_PI*0.5, 0.0, 0.0);
    dfm2::Normalize_Points3(vtx_xyz, 2.5);
  }
  // spatial hash data structure
  std::vector<dfm2::CNodeBVH2> bvh_nodes;
  std::vector<dfm2::CBV3_Sphere<double>> bvh_volumes;
  delfem2::ConstructBVHTriangleMeshMortonCode(
      bvh_nodes, bvh_volumes,
      vtx_xyz, tri_vtx);
  const unsigned int nw = 256;
  const unsigned int nh = 256;
  // above: constant
  // -------------------------------
  // below: changing during execution
  std::vector<float> afRGB(nw * nh * 3, 0.f);
  unsigned int isample = 0;
  dfm2::CMat4d mMVPd_inv;
  auto render = [&](int iw, int ih) {
    std::array<unsigned short, 3> Xi = {
        (unsigned short) (ih * ih),
        (unsigned short) (iw * iw),
        (unsigned short) (isample * isample)};
    const std::pair<dfm2::CVec3d,dfm2::CVec3d> ray = dfm2::RayFromInverseMvpMatrix(
        mMVPd_inv.data(), iw, ih, nw, nh );
    double rd0 = SampleAmbientOcclusion(
        Xi,
        ray.first, ray.second, vtx_xyz, tri_vtx, bvh_nodes, bvh_volumes);
    dfm2::CVec3d r_ave(rd0, rd0, rd0);
    {
      float *ptr = afRGB.data() + (ih * nw + iw) * 3;
      const auto isamplef = static_cast<float>(isample);
      ptr[0] = (isamplef * ptr[0] + r_ave[0]) / (isamplef + 1.f);
      ptr[1] = (isamplef * ptr[1] + r_ave[1]) / (isamplef + 1.f);
      ptr[2] = (isamplef * ptr[2] + r_ave[2]) / (isamplef + 1.f);
    }
  };

  dfm2::opengl::CTexRGB_Rect2D tex;
  {
    tex.width = nw;
    tex.height = nh;
    tex.channels = 3;
    tex.pixel_color.resize(tex.width * tex.height * tex.channels);
  }

  dfm2::glfw::CViewer3 viewer(2.f);
  viewer.window_title = "task04";
  viewer.width = 400;
  viewer.height = 400;
  viewer.camerachange_callbacks.emplace_back( // reset when camera moves
      [&afRGB, &isample] {
        std::fill(afRGB.begin(), afRGB.end(), 0.0);
        isample = 0;
      }
  );
  // --------------
  // start OpenGL

  if (!glfwInit()) { exit(EXIT_FAILURE); }
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  viewer.OpenWindow();
  tex.InitGL();

  while (!glfwWindowShouldClose(viewer.window)) {
    { // inverse of Homography matrix
      const dfm2::CMat4f mP = viewer.GetProjectionMatrix();
      const dfm2::CMat4f mMV = viewer.GetModelViewMatrix();
      const dfm2::CMat4d mMVP = (mP * mMV).cast<double>();
      mMVPd_inv = dfm2::Inverse_Mat4(mMVP.data());
    }
    /*
    for(unsigned int iw=0;iw<nw;++iw){
      for(unsigned int ih=0;ih<nh;++ih){
        render(iw,ih);
      }
    }
     */
    dfm2::parallel_for(nw, nh, render);
    isample++;
    for (unsigned int ih = 0; ih < tex.height; ++ih) {
      for (unsigned int iw = 0; iw < tex.width; ++iw) {
        for (int ic = 0; ic < 3; ++ic) {
          float fc = afRGB[(ih * tex.width + iw) * 3 + ic];
          fc = (fc > 1.f) ? 1.f : fc;
          fc = (fc < 0.f) ? 0.f : fc;
          int ifc = int(fc * 255.f + .5f);
          tex.pixel_color[(ih * tex.width + iw) * 3 + ic] = ifc;
        }
      }
    }
    tex.InitGL();
    ::glfwMakeContextCurrent(viewer.window);
    ::glClearColor(0.8, 1.0, 1.0, 1.0);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D , tex.id_tex);
    tex.Draw_oldGL();
    viewer.SwapBuffers();
    glfwPollEvents();
  }

  glfwDestroyWindow(viewer.window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}


