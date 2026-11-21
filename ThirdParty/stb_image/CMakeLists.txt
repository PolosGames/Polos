add_library(
    stb_image_static
        STATIC stb_image.cpp stb_image.h
)
add_library(
    stb_image::stb_image
        ALIAS stb_image_static
)

target_include_directories(
    stb_image_static
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
if (WIN32)
    target_compile_definitions(
        stb_image_static
            PUBLIC STBI_WINDOWS_UTF8
    )
endif()

set_target_properties(
    stb_image_static
        PROPERTIES
            FOLDER ThirdParty
)
