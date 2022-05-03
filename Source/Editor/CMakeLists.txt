cmake_minimum_required(VERSION 3.21.1)

add_executable(Polos)

target_sources(Polos
    PRIVATE
        editor.cpp
        editor.h
)

target_include_directories(Polos
    PRIVATE
        Editor
)