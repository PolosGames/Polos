cmake_minimum_required(VERSION 3.25)

polos_sources(
    gltf/gltf_loader.cpp
    image/image_loader.cpp
    resource_manager.cpp
)

# Compile the internal glTF entry point into the test executable as well.
polos_tests(resource SOURCES
    test/gltf_loader_test.cpp
    test/loader_test.cpp
    test/resource_manager_test.cpp
    gltf/gltf_loader.cpp
    image/image_loader.cpp
)
