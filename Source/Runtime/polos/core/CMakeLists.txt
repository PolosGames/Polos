cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

generate_versioning_info(
    NAME PolosCore
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    PolosCore
        LANGUAGES CXX
        VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

file(GLOB_RECURSE PolosCore_SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosCore_INC "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
list(REMOVE_ITEM PolosCore_INC "${CMAKE_CURRENT_LIST_DIR}/polos_main.hpp")

add_library(PolosCore SHARED ${PolosCore_SRC} ${PolosCore_INC})

target_include_directories(
    PolosCore
        PRIVATE
            ${POLOS_INC_DIR}
            PolosDataTypes
            ${QUILL_INCLUDE_DIRS}
)
target_precompile_headers(PolosCore REUSE_FROM PolosPCH)

set_target_properties(
    PolosCore PROPERTIES
        PUBLIC_HEADER             "${PolosCore_INC}"
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     True
        POSITION_INDEPENDENT_CODE True
        OUTPUT_NAME               "PolosCore"
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN True
        LINKER_LANGUAGE           CXX
)

install(
    TARGETS PolosCore
    LIBRARY       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    ARCHIVE       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    RUNTIME       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INC_DIR}/core"
)
