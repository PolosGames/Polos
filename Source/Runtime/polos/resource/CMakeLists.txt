cmake_minimum_required(VERSION 3.25)

polos_sources(
    gltf/gltf_loader.cpp
    image/image_loader.cpp
    material/material_loader.cpp
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

polos_tests(gltf-validation SOURCES
    test/gltf_validation_test.cpp
    gltf/gltf_loader.cpp
)

if (BUILD_TESTS)
    # Run image and manager tests independently of glTF decoding tests.
    add_executable(ut-resource-loading
        test/loader_test.cpp
        test/resource_manager_test.cpp
        image/image_loader.cpp
    )
    target_link_libraries(ut-resource-loading PRIVATE polos::polos GTest::gtest_main)
    target_compile_definitions(ut-resource-loading PRIVATE PL_LOGGER_TYPE=App QUILL_DLL_IMPORT BUILD_TESTS)
    add_test(NAME ResourceLoadingTests COMMAND ut-resource-loading)
    set_tests_properties(ResourceLoadingTests PROPERTIES RUN_SERIAL ON TIMEOUT 60 LABELS ut)
endif()
