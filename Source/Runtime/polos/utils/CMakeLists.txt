cmake_minimum_required(VERSION 3.24)

add_library(PolosUtils STATIC)

set(
    PolosUtils_SOURCE
        "guid.cpp"
        "stringid.cpp"
        "type_util.cpp"
)

set(
    PolosUtils_HEADERS
        "alias.h"
        "concepts.h"
        "feature.h"
        "guid.h"
        "macro_util.h"
        "stringid.h"
        "type_util.h"
)

target_sources(
    PolosUtils
        PRIVATE
            ${PolosUtils_SOURCE}
)

target_include_directories(
    PolosUtils
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    PolosPlatform
        PRIVATE
            ${PCH_LIBRARY_NAME}
)

set_target_properties(
    PolosUtils
        PROPERTIES
            PUBLIC_HEADER "${PolosUtils_HEADERS}"
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
            POSITION_INDEPENDENT_CODE TRUE
            OUTPUT_NAME "PolosUtils"
)

target_precompile_headers(
    PolosUtils
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosUtils true)

install(
    TARGETS PolosUtils 
    LIBRARY DESTINATION "${INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INSTALL_DIR}/include"
)
