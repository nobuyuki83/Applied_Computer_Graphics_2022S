#include <cstdio>
#include <cstdlib>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <Eigen/Dense>

#include "delfem2/glfw/viewer3.h"
#include "delfem2/eigen/msh_io.h"

template <
    typename ARRAY2_INDEX,
    typename ARRAY2_COORDS>
void DrawMeshEdge(
    const ARRAY2_COORDS& V0,
    const ARRAY2_INDEX& F0) {
  ::glBegin(GL_LINES);
  for(unsigned int ifc=0;ifc<F0.rows();++ifc){
    unsigned int i0 = F0(ifc,0);
    unsigned int i1 = F0(ifc,1);
    unsigned int i2 = F0(ifc,2);
    ::glVertex3dv(V0.row(i0).data());
    ::glVertex3dv(V0.row(i1).data());
    ::glVertex3dv(V0.row(i1).data());
    ::glVertex3dv(V0.row(i2).data());
    ::glVertex3dv(V0.row(i2).data());
    ::glVertex3dv(V0.row(i0).data());
  }
  ::glEnd();
}

int main() {
  const auto [V0,F0] = delfem2::eigen::ReadTriangleMeshObj(std::string(SOURCE_DIR)+"/../asserts/joint_zz5.obj");
  std::cout << V0.rows() << " " << V0.cols() << std::endl;
  std::cout << F0.rows() << " " << F0.cols() << std::endl;

  delfem2::glfw::CViewer3 viewer;
  viewer.window_title = "task2";
  if ( !glfwInit() ) { exit(EXIT_FAILURE); }
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  viewer.OpenWindow();
  //
  while ( !::glfwWindowShouldClose(viewer.window) ) {
    viewer.DrawBegin_oldGL();
    //
    ::glColor3d(0,0,0);
    DrawMeshEdge(V0,F0);
    //
    ::glfwSwapBuffers(viewer.window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(viewer.window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
