cmake_minimum_required(VERSION 3.24)

# Library target
add_library(stduuid INTERFACE)
target_include_directories(stduuid INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(stduuid::stduuid ALIAS stduuid)
