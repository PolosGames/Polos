cmake_minimum_required(VERSION 3.21)

add_library(glad STATIC src/glad.c)

target_include_directories(glad PUBLIC include)

include(../../cmake/target_build.cmake)

build_options(glad)

#install(TARGETS glad DESTINATION bin)

#install(FILES include/glad/glad.h DESTINATION include/ThirdParty/glad)