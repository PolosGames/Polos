cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

generate_versioning_info(
    NAME system
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    system
    LANGUAGES CXX
    VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        system
    TYPE
        SHARED
    SOURCES
        "engine_subsystem.cpp"
        "engine_subsystem.hpp"
    PRIVATE_DEPS
        polos::polos
        polos::polos_datatypes
)