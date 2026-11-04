cmake_minimum_required(VERSION 3.25)

polos_sources(
    logger.cpp
)

polos_tests(logging SOURCES test/logger_test.cpp)
