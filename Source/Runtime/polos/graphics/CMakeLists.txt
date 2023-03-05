cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosGraphics_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosGraphics_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(
    PolosGraphics STATIC
        ${PolosGraphics_SOURCES} ${PolosGraphics_HEADERS}
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
