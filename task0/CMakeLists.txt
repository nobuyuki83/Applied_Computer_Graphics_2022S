cmake_minimum_required(VERSION 3.12)

#############################
# set C++ detail
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
IF(MSVC)
  set(CMAKE_CXX_FLAGS "/W4")
ELSE()
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -g")
ENDIF()

#############################
# set project name

project(task0)

#############################
# specifying libraries to use

# use opengl
find_package(OpenGL REQUIRED)

# use glfw
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/glfwlib)
find_package(glfw3 REQUIRED)

# use eigen
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../external/eigen/FindEigen3.cmake)
  include(${CMAKE_CURRENT_SOURCE_DIR}/../external/eigen/FindEigen3.cmake) # call script to find eigen3
else()
  FIND_PACKAGE(Eigen3 REQUIRED) # common command to specify eigen  
endif()



########################
# include, build, & link

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

#############################
# showing status of libraries

GET_TARGET_PROPERTY(GLFW_INCLUDE_DIR
  glfw INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "glfw include directory: ${GLFW_INCLUDE_DIR}")

message(STATUS "eigen3 include directory: ${EIGEN3_INCLUDE_DIR}")