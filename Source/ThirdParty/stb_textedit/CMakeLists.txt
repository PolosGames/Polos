cmake_minimum_required(VERSION 3.24)

add_library(stb_textedit_interface INTERFACE)
target_include_directories(stb_textedit_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(stb_textedit_static STATIC stb_textedit.cpp stb_textedit.h)
set_target_properties(stb_textedit_static PROPERTIES FOLDER "ThirdParty")

add_library(stb_textedit::stb_textedit_static ALIAS stb_textedit_static)
add_library(stb_textedit::stb_textedit_interface ALIAS stb_textedit_interface)
