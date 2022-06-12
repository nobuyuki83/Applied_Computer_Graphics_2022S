#include <vector>
#include <Eigen/Geometry>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include "delfem2/msh_io_obj.h"
#include "delfem2/glfw/viewer3.h"
#include "delfem2/glfw/util.h"
#include "delfem2/opengl/old/funcs.h"
#include "delfem2/opengl/old/mshuni.h"

// ---------------------------------

class ArticulatedRigidBodies {
 public:
  Eigen::Matrix<double,8,1> angle{0.,0.,0.,0.,0.,0.,0.,0.}; // rotation angles in radian (0z, 1x,1y,1z, 2x,2y,2z, 3x)
  Eigen::Matrix4d C1, C2, C3, C4; // affine transformation from "local" to "deformed"
  Eigen::Vector3d pos_def;  // deformed output position
  Eigen::Matrix<double,3,8> diff_pos_def; // Jacobian of deformed output position w.r.t. angle
  //
  const Eigen::Vector3d t1{0,0,0}, t2{0,0,1}, t3{0,0,2}, t4{0,0,3};  // position of joints for each rigid bodies
  const Eigen::Vector4d pos_lcl{0.7,0.0,0.2,1};  // local position of end effector
 public:
  /*
   * compute rigid body transformation from angles
   */
  void UpdateTransformations() {
    // compute rotations in "reference" configuration relative to parent rigid body
    const Eigen::Matrix3d R1 = Eigen::AngleAxisd(angle[0], Eigen::Vector3d::UnitZ()).toRotationMatrix();
    const Eigen::Quaterniond R2 =
        Eigen::AngleAxisd(angle[1], Eigen::Vector3d::UnitX()) *
        Eigen::AngleAxisd(angle[2], Eigen::Vector3d::UnitY()) *
        Eigen::AngleAxisd(angle[3], Eigen::Vector3d::UnitZ());
    const Eigen::Quaterniond R3 =
        Eigen::AngleAxisd(angle[4], Eigen::Vector3d::UnitX()) *
        Eigen::AngleAxisd(angle[5], Eigen::Vector3d::UnitY()) *
        Eigen::AngleAxisd(angle[6], Eigen::Vector3d::UnitZ());
    const Eigen::Matrix3d R4 = Eigen::AngleAxisd(angle[7], Eigen::Vector3d::UnitX()).toRotationMatrix();
    // Affine matrices from "local" to "reference"
    const Eigen::Matrix4d T1 = Eigen::Affine3d(Eigen::Translation3d(t1)).matrix();
    const Eigen::Matrix4d T2 = Eigen::Affine3d(Eigen::Translation3d(t2)).matrix();
    const Eigen::Matrix4d T3 = Eigen::Affine3d(Eigen::Translation3d(t3)).matrix();
    const Eigen::Matrix4d T4 = Eigen::Affine3d(Eigen::Translation3d(t4)).matrix();
    // compute affine transformations from "reference" to "deformed"
    const Eigen::Matrix4d A0 = Eigen::Matrix4d::Identity();
    const Eigen::Matrix4d A1 = A0 * T1 * Eigen::Affine3d(R1) * T1.inverse();
    const Eigen::Matrix4d A2 = A1 * T2 * Eigen::Affine3d(R2) * T2.inverse();
    const Eigen::Matrix4d A3 = A2 * T3 * Eigen::Affine3d(R3) * T3.inverse();
    const Eigen::Matrix4d A4 = A3 * T4 * Eigen::Affine3d(R4) * T4.inverse();
    // compute affine transformation from "local" to "deformed"
    C1 = A1 * T1;
    C2 = A2 * T2;
    C3 = A3 * T3;
    C4 = A4 * T4;
    pos_def = (C4 * pos_lcl).head<3>();  // deformed position of end effector
    // deformed positions of joints
    const Eigen::Vector3d jpos0 = (C1 * Eigen::Vector4d(0,0,0,1)).head<3>();
    const Eigen::Vector3d jpos1 = (C2 * Eigen::Vector4d(0,0,0,1)).head<3>();
    const Eigen::Vector3d jpos2 = (C3 * Eigen::Vector4d(0,0,0,1)).head<3>();
    const Eigen::Vector3d jpos3 = (C4 * Eigen::Vector4d(0,0,0,1)).head<3>();
    // rotation of axis
    const Eigen::Matrix3d ri = Eigen::Matrix3d::Identity();
    const Eigen::Matrix3d r0 = ri * Eigen::AngleAxisd(angle[0], Eigen::Vector3d::UnitZ()).toRotationMatrix();
    const Eigen::Matrix3d r1 = r0 * Eigen::AngleAxisd(angle[1], Eigen::Vector3d::UnitX()).toRotationMatrix();
    const Eigen::Matrix3d r2 = r1 * Eigen::AngleAxisd(angle[2], Eigen::Vector3d::UnitY()).toRotationMatrix();
    const Eigen::Matrix3d r3 = r2 * Eigen::AngleAxisd(angle[3], Eigen::Vector3d::UnitZ()).toRotationMatrix();
    const Eigen::Matrix3d r4 = r3 * Eigen::AngleAxisd(angle[4], Eigen::Vector3d::UnitX()).toRotationMatrix();
    const Eigen::Matrix3d r5 = r4 * Eigen::AngleAxisd(angle[5], Eigen::Vector3d::UnitY()).toRotationMatrix();
    const Eigen::Matrix3d r6 = r5 * Eigen::AngleAxisd(angle[6], Eigen::Vector3d::UnitZ()).toRotationMatrix();
    const Eigen::Matrix3d r7 = r6 * Eigen::AngleAxisd(angle[7], Eigen::Vector3d::UnitX()).toRotationMatrix();
    // computing Jacobian of output deformed position
    diff_pos_def.col(0) = (r0 * Eigen::Vector3d(0,0,1)).cross(pos_def - jpos0);
    diff_pos_def.col(1) = (r1 * Eigen::Vector3d(1,0,0)).cross(pos_def - jpos1);
    diff_pos_def.col(2) = (r2 * Eigen::Vector3d(0,1,0)).cross(pos_def - jpos1);
    diff_pos_def.col(3) = (r3 * Eigen::Vector3d(0,0,1)).cross(pos_def - jpos1);
    diff_pos_def.col(4) = (r4 * Eigen::Vector3d(1,0,0)).cross(pos_def - jpos2);
    diff_pos_def.col(5) = (r5 * Eigen::Vector3d(0,1,0)).cross(pos_def - jpos2);
    diff_pos_def.col(6) = (r6 * Eigen::Vector3d(0,0,1)).cross(pos_def - jpos2);
    diff_pos_def.col(7) = (r7 * Eigen::Vector3d(1,0,0)).cross(pos_def - jpos3);
  }
};

class Mesh {
 public:
  std::vector<double> vtx_xyz;
  std::vector<unsigned int> tri_vtx;
 public:
  Mesh(const std::string& fpath){
    delfem2::Read_Obj3(vtx_xyz, tri_vtx, fpath);
    for(auto& v : vtx_xyz){ v *= 0.01; }
  }
  void draw(const Eigen::Matrix4d& C) const {
    ::glPushMatrix();
    ::glMultMatrixd(C.data());
    delfem2::opengl::DrawMeshTri3D_FaceNorm(vtx_xyz, tri_vtx);
    ::glPopMatrix();
  }
};

int main() {
  const Mesh rb1(std::string(SOURCE_DIR)+"/../assets/rb1.obj");
  const Mesh rb2(std::string(SOURCE_DIR)+"/../assets/rb2.obj");
  const Mesh rb3(std::string(SOURCE_DIR)+"/../assets/rb3.obj");
  const Mesh rb4(std::string(SOURCE_DIR)+"/../assets/rb4.obj");
  ArticulatedRigidBodies arb;
  arb.UpdateTransformations();
  // --------------
  // opengl starts here
  delfem2::glfw::CViewer3 viewer(2);
  viewer.window_title = "task07";
  viewer.trans[1] = -1.0;
  viewer.view_rotation = std::make_unique<delfem2::ModelView_Ztop>();
  if (!glfwInit()) { exit(EXIT_FAILURE); }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  viewer.OpenWindow();
  delfem2::opengl::setSomeLighting();
  while (!glfwWindowShouldClose(viewer.window)) {
    double time = glfwGetTime();
    Eigen::Vector3d pos_trg( 1.5+sin(3*time), cos(2*time), 1*cos(time)+1.5 ); // target position
    {
      const Eigen::Matrix<double,8,1> angle0 = arb.angle; // initial angle
      const Eigen::Vector3d pos_def = arb.pos_def; // output position
      const Eigen::Matrix<double,3,8> diff_pos_def = arb.diff_pos_def; // differentiation of output position
      double W0 = 0.5*(pos_def - pos_trg).dot(pos_def - pos_trg); // energy before update

      // write some code below to update arb.angle to decrease energy using Levenberg–Marquardt(LM) algorithm
      // Adjust the coefficient LM algorithm such that the energy decrease after updating "arb.angle".

      // editing ends here
      arb.UpdateTransformations();
      double W1 = 0.5*(arb.pos_def - pos_trg).dot(arb.pos_def - pos_trg); // energy after update
      std::cout << "energy before/after: " << W0 << " -> " << W1 << std::endl;
    }

    // for each bone set relative rotation from parent bone
    // --------------------
    viewer.DrawBegin_oldGL();
    ::glDisable(GL_LIGHTING);
    delfem2::opengl::DrawAxis(1);
    ::glMatrixMode(GL_MODELVIEW);
    // draw target point
    ::glDisable(GL_LIGHTING);
    ::glColor3d(1,0,0);
    delfem2::opengl::DrawSphereAt(
        16,16,0.05,
        pos_trg.x(), pos_trg.y(), pos_trg.z());
    // draw rigid bodies
    ::glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, std::array<float,3>{1.f, 0.5f, 0.5f}.data());
    rb1.draw(arb.C1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, std::array<float,3>{0.5f, 1.0f, 0.5f}.data());
    rb2.draw(arb.C2);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, std::array<float,3>{0.5f, 0.5f, 1.0f}.data());
    rb3.draw(arb.C3);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, std::array<float,3>{0.5f, 0.5f, 0.5f}.data());
    rb4.draw(arb.C4);
    // draw end
    viewer.SwapBuffers();
    glfwPollEvents();
  }
  glfwDestroyWindow(viewer.window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}


