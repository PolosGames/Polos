cmake_minimum_required(VERSION 3.21)

add_library(spdlog STATIC)

target_compile_definitions(
    spdlog
        PRIVATE
            SPDLOG_COMPILED_LIB)

target_include_directories(
    spdlog
        PUBLIC
            include)

target_sources(
    spdlog
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/color_sinks.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/file_sinks.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/stdout_sinks.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cfg.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fmt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/async.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/spdlog.cpp
)

build_options(spdlog false)

# install(TARGETS spdlog DESTINATION bin)