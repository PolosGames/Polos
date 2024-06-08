cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE glm_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/glm/*.h")

add_library(glm_interface INTERFACE ${glm_HEADERS})
target_include_directories(glm_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(glm::glm_interface ALIAS glm_interface)
