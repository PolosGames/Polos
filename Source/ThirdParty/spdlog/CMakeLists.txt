cmake_minimum_required(VERSION 3.24)

add_library(spdlog_interface INTERFACE)
target_include_directories(spdlog_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(spdlog_static STATIC IMPORTED)
set_target_properties(
    spdlog_static
        PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_BUILD_TYPE}/$<IF:$<CONFIG:Release,MinSizeRel>,spdlog.lib,spdlogd.lib>"
            FOLDER            "ThirdParty"
)

add_library(spdlog::spdlog_static ALIAS spdlog_static)
add_library(spdlog::spdlog_interface ALIAS spdlog_interface)
