cmake_minimum_required(VERSION 3.24)

add_library(ImGui STATIC)

set(ImGui_HEADERS
    imconfig.h
    imgui.h
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    "include/internal/imgui_impl_glfw.h"
    "include/internal/imgui_impl_opengl3.h"
    "include/internal/imgui_impl_opengl3_loader.h"
)


set(ImGui_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
)

target_sources(ImGui PRIVATE ${ImGui_SOURCES} ${ImGui_HEADERS})

target_include_directories(
    ImGui
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            "${CMAKE_CURRENT_SOURCE_DIR}/../GLFW/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/internal"
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ImGui PRIVATE cxx_std_20)

build_options(ImGui false)