cmake_minimum_required(VERSION 3.25)

polos_sources(
    event_bus.cpp
)

polos_tests(communication SOURCES test/event_bus_test.cpp)
