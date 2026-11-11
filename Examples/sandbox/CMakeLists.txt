cmake_minimum_required(VERSION 3.25)

project(sandbox)

define_polos_app(
    NAME sandbox
    DIRECTORY Examples
    SOURCES sandbox.cpp helpers.cpp
)