cmake_minimum_required(VERSION 3.12)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
IF(MSVC)
  set(CMAKE_CXX_FLAGS "/W4")
ELSE()
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -g")
ENDIF()

project(task0)

################################

# glfw
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/glfwlib)
find_package(glfw3 REQUIRED)

# opengl
find_package(OpenGL REQUIRED)

# eigen
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/eigenlib)
FIND_PACKAGE(Eigen3 REQUIRED)

################################

include_directories(
)

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME}
  OpenGL::GL
  glfw
  Eigen3::Eigen
)