cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

file(GLOB_RECURSE PolosDataTypes_INC "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

add_library(PolosDataTypes INTERFACE ${PolosDataTypes_INC})

target_include_directories(PolosDataTypes INTERFACE ${POLOS_INC_DIR})
target_precompile_headers(PolosDataTypes REUSE_FROM PolosPCH)

set_target_properties(
    PolosDataTypes PROPERTIES
    PUBLIC_HEADER             "${PolosDataTypes_INC}"
    CXX_STANDARD              ${POLOS_CXX_STANDARD}
    CXX_STANDARD_REQUIRED     True
    OUTPUT_NAME               "PolosDataTypes"
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN True
    LINKER_LANGUAGE           CXX
)

install(
    TARGETS PolosDataTypes
    PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INC_DIR}/datatypes"
)

