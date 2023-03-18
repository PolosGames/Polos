cmake_minimum_required(VERSION 3.24)

add_library(glad_interface INTERFACE)
target_include_directories(glad_interface INTERFACE include)

add_library(glad_static STATIC ${CMAKE_CURRENT_LIST_DIR}/src/glad.c)
set_target_properties(glad_static PROPERTIES FOLDER "ThirdParty")
target_link_libraries(glad_static PRIVATE glad_interface)

add_library(glad::glad_static ALIAS glad_static)
add_library(glad::glad_interface ALIAS glad_interface)

build_options(glad_static false)
