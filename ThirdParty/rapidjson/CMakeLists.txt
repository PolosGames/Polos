cmake_minimum_required(VERSION 3.21.1)

add_library(RapidJSON STATIC)

add_subdirectory(rapidjson)

target_compile_features(RapidJSON
    PRIVATE cxx_std_17
)

target_include_directories(RapidJSON
    PRIVATE rapidjson
    INTERFACE .
)

set_target_properties(RapidJSON PROPERTIES LINKER_LANGUAGE CXX)

build_options(RapidJSON false)