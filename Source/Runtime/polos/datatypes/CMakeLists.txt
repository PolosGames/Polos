cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

file(GLOB_RECURSE datatypes_INC "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

define_polos_module(
    NAME
        datatypes
    TYPE
        INTERFACE
    SOURCES
        "singleton.hpp"
        "singleton.inl.hpp"
    INTERFACE_DEPS
        polos::polos_logging
)
