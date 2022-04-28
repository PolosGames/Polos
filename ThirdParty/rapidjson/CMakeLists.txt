cmake_minimum_required(VERSION 3.21.1)

add_library(RapidJSON INTERFACE)

add_subdirectory(rapidjson)

target_include_directories(RapidJSON
    INTERFACE .
)

set_target_properties(RapidJSON PROPERTIES LINKER_LANGUAGE CXX)