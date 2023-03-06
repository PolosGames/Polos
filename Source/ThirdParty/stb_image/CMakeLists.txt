cmake_minimum_required(VERSION 3.24)

add_library(stb_image_interface INTERFACE)
target_include_directories(stb_image_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(stb_image_static STATIC stb_image.cpp stb_image.h)
set_target_properties(stb_image_static PROPERTIES FOLDER "ThirdParty")

add_library(stb_image::stb_image_static ALIAS stb_image_static)
add_library(stb_image::stb_image_interface ALIAS stb_image_interface)
