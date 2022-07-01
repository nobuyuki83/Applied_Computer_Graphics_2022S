cmake_minimum_required(VERSION 3.12)

#####################################################

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
IF (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc")
ELSE ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
ENDIF ()

####################################

project(task04)

add_definitions(-DPATH_SRC_DIR="${PROJECT_SOURCE_DIR}")

# use glfw
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/glfwlib)
find_package(glfw3 REQUIRED)

# opengl
find_package(OpenGL REQUIRED)

# thread
find_package(Threads REQUIRED)

#################################################

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/delfem2/include
    )

add_executable(${PROJECT_NAME}
    main.cpp
#    main_ans.cpp
    )

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    Threads::Threads
    )
