cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(Polly)

add_executable(Polly editor.cpp)

target_link_libraries(
    Polly
        PRIVATE
            polos::third_party
            polos::core
            polos::logging
)

set_target_properties(
    Polly PROPERTIES
        CXX_STANDARD ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED True
)

build_options(Polly true)
