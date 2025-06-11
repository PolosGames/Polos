cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME memory
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    memory
    LANGUAGES CXX
    VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        memory
    TYPE
        SHARED
    SOURCES
        "src/debug_memory_resource.cpp"
    PUBLIC_DEPS
        polos::logging
    TEST_SOURCES
        "test/debug_memory_resource_test.cpp"
)
