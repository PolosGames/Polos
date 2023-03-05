cmake_minimum_required(VERSION 3.24)

add_library(PolosGraphics STATIC)

set(
    PolosGraphics_SOURCE
        "opengl/gl_context.cpp"
        "opengl/gl_framebuffer.cpp"
        "opengl/gl_shader.cpp"
        "opengl/gl_shader_lib.cpp"
        "opengl/gl_texture.cpp"
        "opengl/gl_vao.cpp"
        "shapes/shapes2d_transform.cpp"
        "renderer.cpp"
)

set(
    PolosGraphics_HEADERS
        "shapes/shapes2d_transform.h"
        "framebuffer.h"
        "graphics_context.h"
        "renderer.h"
        "shader.h"
        "shader_lib.h"
        "texture.h"
        "vao.h"
        "vertex.h"
)

target_sources(
    PolosGraphics
        PRIVATE
            ${PolosGraphics_SOURCE}
)

target_include_directories(
    PolosGraphics
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    PolosGraphics
        PRIVATE
            ${PCH_LIBRARY_NAME}
            glad
            glm
)

set_target_properties(
    PolosGraphics
        PROPERTIES
            PUBLIC_HEADER "${PolosGraphics_HEADERS}"
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
            POSITION_INDEPENDENT_CODE TRUE
            OUTPUT_NAME "PolosGraphics"
)

target_precompile_headers(
    PolosGraphics
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosGraphics true)

install(
    TARGETS PolosGraphics 
    LIBRARY DESTINATION "${INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INSTALL_DIR}/include"
)
