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
        "event_bus/event_bus.cpp"
        "event_bus/event_bus.hpp"
        "events/engine_update.hpp"
        "events/event.cpp"
        "events/event.hpp"
    PRIVATE_DEPS
        polos::polos_logging
        polos::polos_utils
)
