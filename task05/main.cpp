
#include <cstdlib>
#include <math.h>
#include <glad/glad.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include "delfem2/file.h"
#include "delfem2/opengl/funcs.h"

int main() {
  if (!glfwInit()) { exit(EXIT_FAILURE); }
  // set OpenGL's version (note: ver. 2.1 is very old, but I chose because it's simple)
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow *window = ::glfwCreateWindow(500, 500, "task05", nullptr, nullptr);
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

  const GLint iloc_p0 = glGetUniformLocation(shaderProgram, "p0");  // location of variable in the shader program
  const GLint iloc_p1 = glGetUniformLocation(shaderProgram, "p1");  // location of variable in the shader program
  const GLint iloc_p2 = glGetUniformLocation(shaderProgram, "p2");  // location of variable in the shader program
  const GLint iloc_p3 = glGetUniformLocation(shaderProgram, "p3");  // location of variable in the shader program

  glDisable(GL_MULTISAMPLE);
  ::glClearColor(1, 1, 1, 1);  // set the color to fill the frame buffer when glClear is called.
  ::glEnable(GL_DEPTH_TEST);
  while (!::glfwWindowShouldClose(window)) {
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const auto time = static_cast<float>(glfwGetTime());
    glUniform2f(iloc_p0,0.8*sin(1*time),0.8*cos(3*time));
    glUniform2f(iloc_p1,0.8*cos(2*time),0.8*sin(1*time));
    glUniform2f(iloc_p2,0.8*sin(3*time),0.8*cos(2*time));
    glUniform2f(iloc_p3,0.8*cos(1*time),0.8*sin(2*time));
    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity(); // identity transformation
    ::glMatrixMode(GL_MODELVIEW);
    ::glLoadIdentity(); // identity transformation
    ::glUseProgram(shaderProgram);  // use the shader program from here
    ::glBegin(GL_QUADS); // draw a rectangle that cover the entire screen
    ::glVertex2d(-1,-1);
    ::glVertex2d(+1,-1);
    ::glVertex2d(+1,+1);
    ::glVertex2d(-1,+1);
    ::glEnd();
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
