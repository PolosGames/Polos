cmake_minimum_required(VERSION 3.25)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf_v3.h" REQUIRED)

add_subdirectory(quill)
add_subdirectory(stb_image)
add_subdirectory(stb_textedit)
add_subdirectory(tinygltf)

if (ENABLE_PROFILING)
    add_subdirectory(tracy)
endif()
