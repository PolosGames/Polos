cmake_minimum_required(VERSION 3.24)

add_library(glad_interface INTERFACE)
target_include_directories(glad_interface INTERFACE include)

add_library(glad STATIC ${CMAKE_CURRENT_LIST_DIR}/src/glad.c)
set_target_properties(glad PROPERTIES FOLDER "ThirdParty")
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE True)
target_link_libraries(glad PUBLIC glad_interface)

add_library(glad::glad ALIAS glad)
add_library(glad::glad_interface ALIAS glad_interface)

build_options(glad false)
