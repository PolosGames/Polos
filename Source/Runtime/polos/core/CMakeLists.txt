cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME core
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    core
        LANGUAGES CXX
        VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        core
    TYPE
        SHARED
    SOURCES
        "app/application.cpp"
        "app/application.hpp"
        "engine/engine.cpp"
        "engine/engine.hpp"
    PRIVATE_DEPS
        polos::communication
        polos::datatypes
        polos::logging
        polos::utils
)
