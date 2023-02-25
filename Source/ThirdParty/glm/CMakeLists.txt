cmake_minimum_required(VERSION 3.24)

set(GLM_VERSION "0.9.9")

add_subdirectory(include/glm)
add_library(glm::glm ALIAS glm)

build_options(glm_static false)