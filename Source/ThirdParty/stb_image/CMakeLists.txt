cmake_minimum_required(VERSION 3.24)

add_library(stb_image_interface INTERFACE)
target_include_directories(stb_image_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(stb_image_static STATIC stb_image.cpp stb_image.h)
target_link_libraries(stb_image_static PUBLIC stb_image_interface)
set_target_properties(stb_image_static PROPERTIES FOLDER "ThirdParty")
set_target_properties(stb_image_static PROPERTIES POSITION_INDEPENDENT_CODE True)

add_library(stb_image::stb_image ALIAS stb_image_static)
add_library(stb_image::stb_image_interface ALIAS stb_image_interface)
