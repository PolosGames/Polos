cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME utils
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    utils
    LANGUAGES CXX
    VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        utils
    TYPE
        SHARED
    SOURCES
        "time/scoped_timer.cpp"
        "time/scoped_timer.hpp"
        "time/time.cpp"
        "time/time.hpp"
    PRIVATE_DEPS
        polos::logging
        polos::platform
)
