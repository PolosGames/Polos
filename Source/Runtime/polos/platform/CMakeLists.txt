cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosPlatform_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosPlatform_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(
    PolosPlatform STATIC
        ${PolosPlatform_SOURCES} ${PolosPlatform_HEADERS}
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
