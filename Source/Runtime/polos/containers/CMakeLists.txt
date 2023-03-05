cmake_minimum_required(VERSION 3.24)

add_library(PolosContainers STATIC)

set(
    PolosContainers_SOURCE
        "dynamic_array.cpp"
)

set(
    PolosContainers_HEADERS
        "containers.h"
        "delegate.h"
        "dynamic_array.h"
)

target_sources(
    PolosContainers
        PRIVATE
            ${PolosContainers_SOURCE}
)

target_include_directories(
    PolosContainers
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    PolosContainers
        PRIVATE
            ${PCH_LIBRARY_NAME}
)

set_target_properties(
    PolosContainers
        PROPERTIES
            PUBLIC_HEADER "${PolosContainers_HEADERS}"
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
            POSITION_INDEPENDENT_CODE TRUE
            OUTPUT_NAME "PolosContainers"
)

target_precompile_headers(
    PolosContainers
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosContainers true)

install(
    TARGETS PolosContainers 
    LIBRARY DESTINATION "${INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INSTALL_DIR}/include"
)
