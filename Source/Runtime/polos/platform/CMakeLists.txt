cmake_minimum_required(VERSION 3.24)

add_library(PolosPlatform STATIC)

set(
    PolosPlatform_SOURCE
        "linux/linux_timer.cpp"
        "windows/windows_timer.cpp"
        "xplat/x_plat_window_system.cpp"
)

set(
    PolosPlatform_HEADERS
        "platform_detection.h"
)

target_sources(
    PolosPlatform
        PRIVATE
            ${PolosPlatform_SOURCE}
)

target_include_directories(
    PolosPlatform
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    PolosPlatform
        PRIVATE
            ${PCH_LIBRARY_NAME}
)

set_target_properties(
    PolosPlatform
        PROPERTIES
            PUBLIC_HEADER "${PolosPlatform_HEADERS}"
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
            POSITION_INDEPENDENT_CODE TRUE
            OUTPUT_NAME "PolosPlatform"
)

target_precompile_headers(
    PolosPlatform
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosPlatform true)

install(
    TARGETS PolosPlatform 
    LIBRARY DESTINATION "${INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INSTALL_DIR}/include"
)
