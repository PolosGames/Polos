cmake_minimum_required(VERSION 3.25)

polos_sources(assert.cpp string_map.cpp)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()
