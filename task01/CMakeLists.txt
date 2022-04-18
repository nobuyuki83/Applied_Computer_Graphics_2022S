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
project(task1)

# define a macro to get absolute path in C++
add_definitions(-DSOURCE_DIR="${PROJECT_SOURCE_DIR}")

#############################
# specifying libraries to use

# use opengl
find_package(OpenGL REQUIRED)

# use glfw
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/glfwlib)
find_package(glfw3 REQUIRED)

# use eigen
# find_package(Eigen3 REQUIRED) # works only for installation by package managers
FILE(GLOB EIGEN3_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../external/eigen*) # find downloaded eigen directories in 3rd_party
include(${CMAKE_CURRENT_SOURCE_DIR}/../external/FindEigen3.cmake) # call script to find eigen3

########################
# include, build, & link

include_directories(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/../external/delfem2/include
)

add_executable(${PROJECT_NAME}
#  main_ans.cpp
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

GET_TARGET_PROPERTY(EIGEN3_EIGEN_INCLUDE_DIR
  Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "eigen3 include directory: ${EIGEN3_EIGEN_INCLUDE_DIR}")