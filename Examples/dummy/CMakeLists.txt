cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(dummy)

define_polos_app(
    NAME
        dummy
    DIRECTORY
        Examples
    SOURCES
        "dummy_app.cpp"
)
