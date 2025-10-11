cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME platform
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    platform
    LANGUAGES CXX
    VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        platform
    TYPE
        SHARED
    SOURCES
        "src/dll_main.cpp"
        "src/window_manager.cpp"
    PUBLIC_DEPS
        glfw
        Vulkan::Vulkan
        polos::communication
        polos::logging
        polos::rendering
)