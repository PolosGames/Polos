cmake_minimum_required(VERSION 3.25)

polos_sources(
    base_live_layer.cpp
    engine_layer.cpp
    engine.cpp
    main_loop.cpp
)

# MainLoop is internal to the runtime library.
polos_tests(core SOURCES test/main_loop_test.cpp main_loop.cpp)

if (BUILD_TESTS AND BUILD_INTEGRATION_TESTS)
    include(GoogleTest)
    add_executable(it-engine-smoke test/engine_integration_test.cpp)
    target_link_libraries(it-engine-smoke PRIVATE polos::polos GTest::gtest_main)
    target_compile_definitions(it-engine-smoke PRIVATE PL_LOGGER_TYPE=App QUILL_DLL_IMPORT)
    set_target_properties(it-engine-smoke PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/integration")

    set(SMOKE_RESOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/integration/Resources")
    configure_file(test/integration/resources.yaml "${SMOKE_RESOURCE_DIR}/resources.yaml" COPYONLY)

    gtest_add_tests(TARGET it-engine-smoke TEST_LIST INTEGRATION_TESTS
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/integration")
    set_tests_properties(${INTEGRATION_TESTS} PROPERTIES
        LABELS it RUN_SERIAL ON TIMEOUT 30
        FAIL_REGULAR_EXPRESSION "Validation Error;VUID-")
endif()
