cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosUtils_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosUtils_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(
    PolosUtils STATIC
        ${PolosUtils_SOURCES} ${PolosUtils_HEADERS}
)

target_include_directories(
    PolosUtils
        PRIVATE
            ${POLOS_INCLUDE_DIR}
)

target_link_libraries(
    PolosPlatform
        PRIVATE
            ${PCH_LIBRARY_NAME}
)

set_target_properties(
    PolosUtils
        PROPERTIES
            PUBLIC_HEADER             "${PolosUtils_HEADERS}"
            CXX_STANDARD              20
            CXX_STANDARD_REQUIRED     True
            POSITION_INDEPENDENT_CODE True
            OUTPUT_NAME               "PolosUtils"
)

target_precompile_headers(
    PolosUtils
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosUtils true)

install(
    TARGETS PolosUtils 
    LIBRARY       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    ARCHIVE       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    RUNTIME       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INCLUDES_DIR}/utils"
)
