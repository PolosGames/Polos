cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME rendering
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    rendering
        LANGUAGES CXX
        VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        rendering
    TYPE
        SHARED
    SOURCES
        "src/vk_instance.cpp"
    PUBLIC_DEPS
        glfw
        Vulkan::Vulkan
        polos::logging
        polos::filesystem
)
