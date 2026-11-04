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

    set(SMOKE_RESOURCE_DIR "$<TARGET_FILE_DIR:it-engine-smoke>/Resources")
    set(SMOKE_SHADER_DIR "${CMAKE_BINARY_DIR}/Source/Resource/Shaders")
    add_dependencies(it-engine-smoke compile_shaders)
    add_custom_command(TARGET it-engine-smoke POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SMOKE_RESOURCE_DIR}/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/test/integration/resources.json"
            "${SMOKE_RESOURCE_DIR}/resources.json"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SMOKE_SHADER_DIR}/quad.vert.spv" "${SMOKE_SHADER_DIR}/quad.frag.spv"
            "${SMOKE_RESOURCE_DIR}/Shaders"
        VERBATIM)
    set_property(TARGET it-engine-smoke APPEND PROPERTY LINK_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/test/integration/resources.json"
        "${SMOKE_SHADER_DIR}/quad.vert.spv" "${SMOKE_SHADER_DIR}/quad.frag.spv")

    gtest_add_tests(TARGET it-engine-smoke TEST_LIST INTEGRATION_TESTS
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/integration")
    set_tests_properties(${INTEGRATION_TESTS} PROPERTIES
        LABELS it RUN_SERIAL ON TIMEOUT 30
        FAIL_REGULAR_EXPRESSION "Validation Error;VUID-;Failed to present the image")
endif()
