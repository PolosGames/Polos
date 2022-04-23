cmake_minimum_required(VERSION 3.21.1)

target_sources(Polos
    PRIVATE
        editor.cpp
        editor.h
)

target_include_directories(Polos
    PRIVATE
        Editor
)