cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME resource
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    resource
    LANGUAGES CXX
    VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

define_polos_module(
    NAME
        resource
    TYPE
        OBJECT
    SOURCES
        "src/image_loader.cpp"
    PRIVATE_DEPS
        polos::logging
)

target_include_directories(resource PRIVATE ${TINYGLTF_INCLUDE_DIRS})
