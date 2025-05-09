cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

define_polos_fixture(
    NAME
        polos_systems_fixture
    SOURCES
        "polos_systems_fixture.cpp"
        "polos_systems_fixture.hpp"
    DEPS
        polos::polos_logging
)
