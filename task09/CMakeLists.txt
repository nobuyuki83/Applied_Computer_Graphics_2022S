cmake_minimum_required(VERSION 3.12)

#########################################################

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
if(MSVC)
  set(CMAKE_CXX_FLAGS "/W4")
else()
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif()

##########################################################

project(cppmodule)

# use pybind11
set(PYBIND11_DIR "${PROJECT_SOURCE_DIR}/../external/pybind11")
add_subdirectory(${PYBIND11_DIR} pybind11)

# use eigen
# find_package(Eigen3 REQUIRED) # works only for installation by package managers
FILE(GLOB EIGEN3_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../external/eigen*) # find downloaded eigen directories in 3rd_party
include(${CMAKE_CURRENT_SOURCE_DIR}/../external/FindEigen3.cmake) # call script to find eigen3

########################

include_directories(${PROJECT_NAME}
    ${EIGEN3_INCLUDE_DIR}
)

###########################################

pybind11_add_module(${PROJECT_NAME}
#   main_ans.cpp)
    main.cpp)

file(TO_NATIVE_PATH "${PROJECT_BINARY_DIR}" NATIVE_PROJECT_BINARY_DIR)
file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}" NATIVE_PROJECT_SOURCE_DIR)

if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
    COMMAND copy 
      "${NATIVE_PROJECT_BINARY_DIR}\\${CMAKE_CFG_INTDIR}\\${PROJECT_NAME}.*.pyd"   
      ${NATIVE_PROJECT_SOURCE_DIR}\\
    COMMENT "Post Event"
  )
else()
  add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy 
      ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PROJECT_NAME}.*
      ${PROJECT_SOURCE_DIR}/
    COMMENT "Post Event"
  )
endif()
