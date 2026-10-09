cmake_minimum_required(VERSION 3.25)

add_library(tinygltf_interface INTERFACE)
target_include_directories(tinygltf_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(tinygltf_interface INTERFACE ${TINYGLTF_INCLUDE_DIRS})

add_library(tinygltf_static STATIC tiny_gltf3.cpp)
target_link_libraries(tinygltf_static PUBLIC tinygltf_interface)
set_target_properties(tinygltf_static PROPERTIES FOLDER "ThirdParty")
set_target_properties(tinygltf_static PROPERTIES POSITION_INDEPENDENT_CODE True)

add_library(tinygltf::tinygltf ALIAS tinygltf_static)
add_library(tinygltf::tinygltf_interface ALIAS tinygltf_interface)
