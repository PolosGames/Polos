cmake_minimum_required(VERSION 3.24)

set(
    POLOS_INCLUDE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(PCH_LIBRARY_NAME RuntimePCH)
add_library(${PCH_LIBRARY_NAME} STATIC "polos.cpp")

target_precompile_headers(
    ${PCH_LIBRARY_NAME}
        PUBLIC
            polos/polos_pch.h
)

set_target_properties(
    ${PCH_LIBRARY_NAME}
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
)

target_link_libraries(
    ${PCH_LIBRARY_NAME}
        PUBLIC
            glm
)

target_include_directories(
    ${PCH_LIBRARY_NAME}
        INTERFACE
            ${POLOS_INCLUDE_DIR}
)

add_compile_definitions(
    GLFW_INCLUDE_NONE
    _CRT_SECURE_NO_WARNINGS
)

# Runtime library should be used for linking to the application itself.

add_library(Runtime INTERFACE)

set_target_properties(
    Runtime
        PROPERTIES
            OUTPUT_NAME "PolosRuntime"
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
)

add_subdirectory(polos/containers)
add_subdirectory(polos/core)
add_subdirectory(polos/graphics)
add_subdirectory(polos/gui)
add_subdirectory(polos/platform)
add_subdirectory(polos/utils)

target_link_libraries(
    Runtime
        INTERFACE
            PolosCore
            PolosContainers
            PolosGraphics
            PolosGUI
            PolosPlatform
            PolosUtils
)

target_include_directories(
    Runtime
        INTERFACE
            ${POLOS_INCLUDE_DIR}
)
