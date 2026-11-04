cmake_minimum_required(VERSION 3.25)

# These tests exercise both macro branches in every build, without depending on the engine.
include(GoogleTest)
foreach(MODE debug release)
    set(ASSERT_TARGET ut-assert-${MODE})
    add_executable(${ASSERT_TARGET} assert_test.cpp ../assert.cpp)
    target_compile_features(${ASSERT_TARGET} PRIVATE cxx_std_23)
    target_include_directories(${ASSERT_TARGET} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../.."
        "${POLOS_CONFIG_INCLUDE_DIR}")
    target_compile_definitions(${ASSERT_TARGET} PRIVATE POLOS_ENGINE_EXPORTS)
    target_link_libraries(${ASSERT_TARGET} PRIVATE GTest::gtest_main)
    build_options(${ASSERT_TARGET} true)
    gtest_discover_tests(${ASSERT_TARGET} TEST_PREFIX "Assert.${MODE}."
        PROPERTIES TIMEOUT 60 LABELS ut)
endforeach()

target_compile_options(ut-assert-debug PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/UNDEBUG>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-UNDEBUG>")
target_compile_definitions(ut-assert-release PRIVATE NDEBUG)

polos_tests(utils SOURCES utils_test.cpp)
