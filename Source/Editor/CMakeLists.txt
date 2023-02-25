cmake_minimum_required(VERSION 3.24)

add_executable(Polos)

target_sources(Polos
    PRIVATE
        editor.cpp
)

target_include_directories(Polos
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)