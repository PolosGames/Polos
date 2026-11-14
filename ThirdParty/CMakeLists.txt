cmake_minimum_required(VERSION 3.25)

add_subdirectory(quill)
add_subdirectory(stb_image)
add_subdirectory(stb_textedit)
add_subdirectory(fastgltf)

if (ENABLE_PROFILING)
    add_subdirectory(tracy)
endif()
