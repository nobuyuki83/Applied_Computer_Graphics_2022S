#include <cstdio>
#include <cstdlib>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <filesystem>
#include <Eigen/Dense>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "delfem2/glfw/viewer2.h"

Eigen::Matrix<double,4,4,Eigen::RowMajor> GetHomographicTransformation(
    const double c1[4][2])
{
  const double c0[4][2] = {
      {-0.5,-0.5},
      {+0.5,-0.5},
      {+0.5,+0.5},
      {-0.5,+0.5} };
  Eigen::Matrix<double,4,4,Eigen::RowMajor> m;
  // set identity
    m <<
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1;
  // write some code to compute the 4x4 Homographic transformation matrix `m`;
  // `m` should transfer :
  // (c0[0][0],c0[][1],z) -> (c1[0][0],c1[0][1],z),`
  // (c0[1][0],c0[][1],z) -> (c1[1][0],c1[1][1],z),
  // (c0[2][0],c0[][1],z) -> (c1[2][0],c1[2][1],z),
  // (c0[3][0],c0[][1],z) -> (c1[3][0],c1[3][1],z)
  return m;
}

int main() {

  std::string path = std::string(SOURCE_DIR) + "/../assets/ada.png";
  std::vector<char> img_data;
  int img_width, img_height, img_channels;
  { // load image data using stb library
    stbi_set_flip_vertically_on_load(true);
    assert( std::filesystem::exists(path.c_str()) );
    unsigned char *img = stbi_load(
        path.c_str(),
        &img_width, &img_height, &img_channels, 0);
    assert(img_width > 0 && img_height > 0);
    std::cout << "image size: " << img_width << " " << img_height << " " << img_channels << std::endl;
    img_data.assign(img,img+img_width*img_height*img_channels);
    stbi_image_free(img);
  }

  if (!glfwInit()) { exit(EXIT_FAILURE); }
  // set OpenGL's version (note: ver. 2.1 is very old, but I chose because it's simple)
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow *window = ::glfwCreateWindow(500, 500, "task01", nullptr, nullptr);
  if (!window) { // exit if failed to create window
    ::glfwTerminate();
    exit(EXIT_FAILURE);
  }
  ::glfwMakeContextCurrent(window); // working on this window below

  // set image to texture calling OpenGL's function
  // if you are interested in OpenGL's texture, look: https://learnopengl.com/Getting-started/Textures
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               static_cast<int>(img_width),
               static_cast<int>(img_height),
               0, GL_RGBA, GL_UNSIGNED_BYTE,
               img_data.data());


  ::glClearColor(1, 1, 1, 1);
  ::glEnable(GL_DEPTH_TEST);
  ::glEnable(GL_POLYGON_OFFSET_FILL);
  ::glPolygonOffset(1.1f, 4.0f);
  while (!::glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    const double corners[4][2] = {
        {-0.5, -0.5},
        {+0.5, -0.5},
        {+0.5 - 0.4*cos(1*time), +0.5 - 0.4*sin(3*time)},
        {-0.5 + 0.4*sin(2*time), +0.5 + 0.4*cos(5*time)} };

    Eigen::Matrix<double,4,4,Eigen::ColMajor> modelview_matrix = GetHomographicTransformation(corners);

    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // set projection matrix
    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity();

    // draw red points
    ::glDisable(GL_TEXTURE_2D);
    ::glDisable(GL_LIGHTING);
    ::glMatrixMode(GL_MODELVIEW);
    ::glLoadIdentity();
    ::glColor3d(1,0,0);
    ::glPointSize(10);
    ::glBegin(GL_POINTS);
    ::glVertex2dv(corners[0]);
    ::glVertex2dv(corners[1]);
    ::glVertex2dv(corners[2]);
    ::glVertex2dv(corners[3]);
    ::glEnd();

    // set model view matrix
    ::glMatrixMode(GL_MODELVIEW);
    ::glLoadIdentity();
    ::glMultMatrixd(modelview_matrix.data());
    ::glEnable(GL_TEXTURE_2D);
    ::glColor3d(1,1,1);
    ::glBegin(GL_QUADS);
    ::glTexCoord2d(0,0);
    ::glVertex2d(-0.5,-0.5);
    ::glTexCoord2d(1,0);
    ::glVertex2d(+0.5,-0.5);
    ::glTexCoord2d(1,1);
    ::glVertex2d(+0.5,+0.5);
    ::glTexCoord2d(0,1);
    ::glVertex2d(-0.5,+0.5);
    ::glEnd();
    //
    ::glfwSwapBuffers(window);
    ::glfwPollEvents();
  }
  ::glfwDestroyWindow(window);
  ::glfwTerminate();
  exit(EXIT_SUCCESS);
}
