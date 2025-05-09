cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME logging
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    logging
    LANGUAGES CXX
    VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        logging
    TYPE
        SHARED
    SOURCES
        "logger.cpp"
        "logger.hpp"
        "log_macros.hpp"
    PUBLIC_DEPS
        quill::quill_shared
    PRIVATE_DEPS
        polos::polos_platform
)