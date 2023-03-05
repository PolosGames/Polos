cmake_minimum_required(VERSION 3.24)

add_library(PolosGUI STATIC)

set(
    PolosGUI_SOURCE
        "imgui/imgui_gui.cpp"
)

set(
    PolosGUI_HEADERS
        "gui.h"
)

target_sources(
    PolosGUI
        PRIVATE
            ${PolosGUI_SOURCE}
)

target_include_directories(
    PolosGUI
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    PolosGUI
        PRIVATE
            ${PCH_LIBRARY_NAME}
)

set_target_properties(
    PolosGUI
        PROPERTIES
            PUBLIC_HEADER "${PolosGUI_HEADERS}"
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
            POSITION_INDEPENDENT_CODE TRUE
            OUTPUT_NAME "PolosGUI"
)

target_precompile_headers(
    PolosGUI
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosGUI true)

install(
    TARGETS PolosGUI 
    LIBRARY DESTINATION "${INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INSTALL_DIR}/include"
)
