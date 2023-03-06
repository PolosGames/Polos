cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosCore_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosCore_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")
list(REMOVE_ITEM PolosCore_HEADERS "${CMAKE_CURRENT_LIST_DIR}/entry_point.h")

add_library(
    PolosCore STATIC
        ${PolosCore_SOURCES} ${PolosCore_HEADERS}
)

target_include_directories(
    PolosCore
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/../..
)

target_link_libraries(
    PolosCore
        PRIVATE
            ${PCH_LIBRARY_NAME}
            spdlog::spdlog_interface
            glm::glm_interface
            glad::glad_interface
            fmod::FMOD_interface
)

set_target_properties(
    PolosCore
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
            POSITION_INDEPENDENT_CODE TRUE
            OUTPUT_NAME "PolosCore"
)

target_precompile_headers(
    PolosCore
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosCore true)

install(
    TARGETS PolosCore 
    LIBRARY DESTINATION "${INSTALL_DIR}"
    ARCHIVE DESTINATION "${INSTALL_DIR}/include"
)