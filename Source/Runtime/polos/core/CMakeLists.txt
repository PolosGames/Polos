cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosCore_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosCore_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")
list(REMOVE_ITEM PolosCore_HEADERS "${CMAKE_CURRENT_LIST_DIR}/entry_point.h")

add_library(
    PolosCore STATIC
        ${PolosCore_SOURCES} ${PolosCore_HEADERS}
)

target_link_libraries(
    PolosCore
        PRIVATE
            ${POLOS_INCLUDE_LIB}
            spdlog::spdlog_interface
            glm::glm_interface
            glad::glad_interface
            fmod::fmod_interface
)

set_target_properties(
    PolosCore
        PROPERTIES
            PUBLIC_HEADER             "${PolosCore_HEADERS}"
            CXX_STANDARD              20
            CXX_STANDARD_REQUIRED     True
            POSITION_INDEPENDENT_CODE True
            OUTPUT_NAME               "PolosCore"
)

target_precompile_headers(
    PolosCore
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosCore true)

install(
    TARGETS PolosCore 
    LIBRARY       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    ARCHIVE       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    RUNTIME       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INCLUDES_DIR}/core"
)