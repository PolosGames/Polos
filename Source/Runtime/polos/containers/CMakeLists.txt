cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosContainers_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosContainers_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(
    PolosContainers STATIC
        ${PolosContainers_SOURCES} ${PolosContainers_HEADERS}
)

target_include_directories(
    PolosContainers
        PRIVATE
            ${POLOS_INCLUDE_DIR}
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
            POSITION_INDEPENDENT_CODE True
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
