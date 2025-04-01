cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(Polly)

add_executable(Polly editor.cpp)

target_include_directories(Polly PRIVATE ${POLOS_INC_DIR})
target_link_libraries(
    Polly
        PRIVATE
            ${PolosThirdParty}
            #PolosCore
)

set_target_properties(
    Polly PROPERTIES
        CXX_STANDARD ${POLOS_CXX_STANDARD}
)