cmake_minimum_required(VERSION 3.21)

add_library(ImGui STATIC)

set(ImGui_HEADERS
    imconfig.h
    imgui.h
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
)


set(ImGui_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
)

target_sources(ImGui PRIVATE ${ImGui_SOURCES} ${ImGui_HEADERS})

target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(ImGui PRIVATE cxx_std_20)

build_options(ImGui false)