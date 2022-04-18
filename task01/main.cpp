#include <cstdio>
#include <cstdlib>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "delfem2/glfw/viewer3.h"
#include "delfem2/eigen/msh_io.h"
#include "delfem2/eigen_opengl/funcs.h"
#include "delfem2/opengl/old/funcs.h"

int main() {

  Eigen::Matrix<double, -1, 3, Eigen::RowMajor> V0;  // same as Eigen::MatrixXd
  Eigen::Matrix<unsigned int, -1, 3, Eigen::RowMajor> F0;  // same as Eigen::MatrixXui
  delfem2::eigen::ReadTriangleMeshObj<double>(  // read mesh in obj format
      V0, F0,
      std::string(SOURCE_DIR) + "/../assets/bunny.obj");

  /*
   * Problem 1: scale & translate the vertex coordinates `V0`
   * Make the axis aligned bounding box's center located at the origin
   * the code is probably up to 5 lines
   */

  Eigen::Matrix<double,4,4,Eigen::RowMajor> modelview_matrix;
  modelview_matrix <<
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1;

  /*
   * Problem 2: rotate the model 90 degree in x-axis
   * edit `modelview_matrix`
   * the code is probably up to 5 lines
   */

  Eigen::Matrix<double,4,4,Eigen::RowMajor> projection_matrix;
  projection_matrix <<
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1;

  /*
   * Problem 3: view the model from (0, 0, 10) with 50 mm lens.
   * edit 'modelview_matrix' and 'projection matrix'
   * the code is probably up to 5 lines
   */

  if (!glfwInit()) { exit(EXIT_FAILURE); }
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow *window = ::glfwCreateWindow(
      500, 500, "task1",
      nullptr, nullptr);
  if (!window) { // exit if failed to create window
    ::glfwTerminate();
    exit(EXIT_FAILURE);
  }
  ::glfwMakeContextCurrent(window); // working on this window
  delfem2::opengl::setSomeLighting();
  //
  ::glClearColor(1, 1, 1, 1);
  ::glEnable(GL_DEPTH_TEST);
  ::glEnable(GL_POLYGON_OFFSET_FILL);
  ::glPolygonOffset(1.1f, 4.0f);
  while (!::glfwWindowShouldClose(window)) {
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // set model view matrix
    ::glMatrixMode(GL_MODELVIEW);
    ::glLoadIdentity();
    ::glMultMatrixd(modelview_matrix.transpose().eval().data());
    // set projection matrix
    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity();
    {
      Eigen::Matrix<double,4,4,Eigen::RowMajor> m;
      m << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1;
      ::glMultMatrixd((m*projection_matrix).transpose().eval().data());
    }
    //
    ::glDisable(GL_LIGHTING);
    ::glColor3d(0, 0, 0);
    delfem2::eigen_opengl::DrawMeshTri3_Edge(V0, F0);
    ::glEnable(GL_LIGHTING);
    delfem2::eigen_opengl::DrawMeshTri3_FaceFlatNorm(V0, F0);
    //
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
