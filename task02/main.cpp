
#include <cstdlib>
#include <glad/glad.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "delfem2/file.h"
#include "delfem2/msh_io_obj.h"
#include "delfem2/mshmisc.h"
#include "delfem2/opengl/funcs.h"
#include "delfem2/opengl/old/mshuni.h"

int main() {

  std::vector<double> vtx_xyz;
  std::vector<unsigned int> tri_vtx;
  delfem2::Read_Obj3(
      vtx_xyz, tri_vtx,
      std::string(SOURCE_DIR)+"/../assets/armadillo1.obj");
  std::cout << "number of vertex: " << vtx_xyz.size() / 3 << std::endl;
  std::cout << "number of triangles: " << tri_vtx.size() / 3 << std::endl;

  if (!glfwInit()) { exit(EXIT_FAILURE); }
  // set OpenGL's version (note: ver. 2.1 is very old, but I chose because it's simple)
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow *window = ::glfwCreateWindow(500, 500, "task02", nullptr, nullptr);
  if (!window) { // exit if failed to create window
    ::glfwTerminate();
    exit(EXIT_FAILURE);
  }
  ::glfwMakeContextCurrent(window); // working on this window below
  //
  if (!gladLoadGL()) {     // glad: load all OpenGL function pointers
    printf("Something went wrong in loading OpenGL functions!\n");
    exit(-1);
  }

  int shaderProgram;
  {
    std::string vrt_path = std::string(SOURCE_DIR) + "/shader.vert";
    std::string frg_path = std::string(SOURCE_DIR) + "/shader.frag";
    std::string vrt = delfem2::LoadFile(vrt_path); // read source code of vertex shader program
    std::string frg = delfem2::LoadFile(frg_path); // read source code of fragment shader program
    shaderProgram = delfem2::opengl::setUpGLSL(vrt, frg); // compile the shader on GPU
  }

  GLint iloc = glGetUniformLocation(shaderProgram, "cam_z_pos");  // location of variable in the shader program

  ::glClearColor(1, 1, 1, 1);  // set the color to fill the frame buffer when glClear is called.
  ::glEnable(GL_DEPTH_TEST);
  while (!::glfwWindowShouldClose(window)) {
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const double time = glfwGetTime();
    const double cam_z_pos = sin(time);  // variable on CPU
    glUniform1f(iloc,float(cam_z_pos));  // send variable to GPU

    ::glUseProgram(shaderProgram);  // use the shapder program from here
    delfem2::opengl::DrawMeshTri3D_FaceNorm(vtx_xyz, tri_vtx);  // draw triangle mesh

    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
