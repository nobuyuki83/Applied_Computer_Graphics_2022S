#include <cstdio>
#include <cstdlib>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "delfem2/glfw/viewer3.h"
#include "delfem2/eigen/msh_io.h"
#include "delfem2/eigen_opengl/funcs.h"
#include "delfem2/opengl/old/funcs.h"

class RigidBone {
 public:
  void Initialize() {
    delfem2::eigen::ReadTriangleMeshObj(
        V,F,
        std::string(SOURCE_DIR)+"/../assets/joint_zz5.obj");
    std::cout << V.rows() << " " << V.cols() << std::endl;
    std::cout << F.rows() << " " << F.cols() << std::endl;
  }
  void Draw() const {
    ::glDisable(GL_LIGHTING);
    ::glColor3fv( color.data() );  // specify color
    delfem2::eigen_opengl::DrawMeshTri3_Edge(V,F);
    ::glColor3d(0.8, 0.8, 0.8);
    ::glEnable(GL_LIGHTING);
    delfem2::eigen_opengl::DrawMeshTri3_FaceFlatNorm(V,F);
  }
 public:
  Eigen::Matrix<double, 4, 4, Eigen::RowMajor> affine_matrix;   // affine matrix
  Eigen::Matrix<double, -1, 3, Eigen::RowMajor> V;  // vertex's coordinates
  Eigen::Matrix<unsigned int, -1, 3, Eigen::RowMajor> F;  // triangle's vertex
  std::array<float,3> color{0.f, 0.f, 0.f};
};

int main() {

  RigidBone bone;
  bone.Initialize();

  delfem2::glfw::CViewer3 viewer;
  viewer.window_title = "task2";
  if ( !glfwInit() ) { exit(EXIT_FAILURE); }
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  viewer.OpenWindow();
  delfem2::opengl::setSomeLighting();
  //
  while ( !::glfwWindowShouldClose(viewer.window) ) {
    viewer.DrawBegin_oldGL();
    //
    bone.Draw();
    //
    ::glfwSwapBuffers(viewer.window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(viewer.window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
