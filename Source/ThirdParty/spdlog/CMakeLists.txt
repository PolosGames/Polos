cmake_minimum_required(VERSION 3.24)

add_library(spdlog_interface INTERFACE)
target_include_directories(spdlog_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

set(SPDLOG_LIB_NAME)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(SPDLOG_LIB_NAME spdlog.lib)
else()
    set(SPDLOG_LIB_NAME spdlogd.lib)
endif()

add_library(spdlog_static STATIC IMPORTED GLOBAL)
set_target_properties(
    spdlog_static
        PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_BUILD_TYPE}/${SPDLOG_LIB_NAME}"
            FOLDER            "ThirdParty"
)

add_library(spdlog::spdlog_static ALIAS spdlog_static)
add_library(spdlog::spdlog_interface ALIAS spdlog_interface)
