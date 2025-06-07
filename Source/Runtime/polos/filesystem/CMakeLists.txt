cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME filesystem
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    filesystem
        LANGUAGES CXX
        VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        filesystem
    TYPE
        SHARED
    SOURCES
        "src/file_manip.cpp"
    PRIVATE_DEPS
        polos::logging
    TEST_SOURCES
        "test/file_manip_test.cpp"
    TEST_DATA
        "test/test_file.test"
)
