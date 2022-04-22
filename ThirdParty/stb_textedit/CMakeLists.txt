cmake_minimum_required(VERSION 3.21)

add_library(stb_textedit STATIC)

target_sources(stb_textedit PRIVATE stb_textedit.cpp stb_textedit.h)

target_include_directories(stb_textedit INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})