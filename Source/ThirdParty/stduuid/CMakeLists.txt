cmake_minimum_required(VERSION 3.24)

# Library target
add_library(stduuid_interface INTERFACE)
target_include_directories(stduuid_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(stduuid::stduuid_interface ALIAS stduuid_interface)
