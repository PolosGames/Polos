cmake_minimum_required(VERSION 3.21.1)

project(
    Polos
    VERSION 0.1.0
)

add_executable(Polos)

target_sources(Polos
    PRIVATE
        editor.cpp
)

target_include_directories(Polos
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)