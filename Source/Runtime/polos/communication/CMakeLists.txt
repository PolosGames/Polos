cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME communication
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    communication
        LANGUAGES CXX
        VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        communication
    TYPE
        SHARED
    SOURCES
        "src/dll_main.cpp"
        "src/event_bus.cpp"
    PUBLIC_DEPS
        polos::logging
    PRIVATE_DEPS
        polos::utils
    TEST_SOURCES
        "test/event_bus_test.cpp"
)
