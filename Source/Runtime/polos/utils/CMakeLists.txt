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
        STATIC
    SOURCES
        "src/scoped_timer.cpp"
        "src/time.cpp"
    PRIVATE_DEPS
        polos::logging
    TEST_SOURCES
        "test/time_test.cpp"
)
