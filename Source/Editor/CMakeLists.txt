cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(Polly)

add_executable(Polly editor.cpp)

target_include_directories(Polly PRIVATE ${POLOS_INC_DIR})
target_link_libraries(
    Polly
        PRIVATE
            polos::polos_third_party
            polos::polos_core
            polos::polos_logging
)

set_target_properties(
    Polly PROPERTIES
        CXX_STANDARD ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED True
)

build_options(Polly true)
