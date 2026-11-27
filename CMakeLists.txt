cmake_minimum_required(VERSION 3.25)
if (POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0177 NEW)
endif()
# The LINUX platform variable used by the build requires CMake 3.25.

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" POLOS_VERSION)
string(STRIP "${POLOS_VERSION}" POLOS_VERSION)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")

project(Polos
    VERSION ${POLOS_VERSION}
    DESCRIPTION "Game engine for PolosGames written in C++23"
    LANGUAGES C CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(POLOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
# Keep SDK destinations relative so a staged SDK can be moved as a unit.
if (POLOS_INSTALL_DIR)
    cmake_path(ABSOLUTE_PATH POLOS_INSTALL_DIR BASE_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
        NORMALIZE OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX
    )
endif()
set(POLOS_INSTALL_DIR ".")

option(ENABLE_PROFILING        "Enable Tracy instrumentation" OFF)
option(GUI_USE_IMGUI           "Use ImGui for default Gui implementation" ON)
option(BUILD_EXAMPLES          "Will build all example projects under Examples directory." ON)
option(BUILD_TESTS             "Build all unit and component tests for the runtime" OFF)
option(BUILD_INTEGRATION_TESTS "Build window/Vulkan integration tests (requires BUILD_TESTS)" OFF)
option(ENABLE_CLANG_TIDY       "Enable clang-tidy static analysis during builds" OFF)

set(POLOS_EXAMPLE "" CACHE STRING "Build only this example target when set")
set(POLOS_INSTALL_INC_DIR "include")
set(POLOS_INSTALL_LIB_DIR "${POLOS_INSTALL_DIR}")

message(STATUS "[POLOS] ENABLE_PROFILING: ${ENABLE_PROFILING}")
message(STATUS "[POLOS] GUI_USE_IMGUI: ${GUI_USE_IMGUI}")
message(STATUS "[POLOS] BUILD_TESTS: ${BUILD_TESTS}")
message(STATUS "[POLOS] CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "[POLOS] ENABLE_CLANG_TIDY: ${ENABLE_CLANG_TIDY}")

if (ENABLE_CLANG_TIDY)
    include(CMake/Tidy.cmake)
endif()

set(POLOS_CONFIG_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated/include")
configure_file(polos_api.hpp.in "${POLOS_CONFIG_INCLUDE_DIR}/polos/polos_api.hpp" COPYONLY)

find_package(Git QUIET)
include(CMake/BuildOptions.cmake)
include(CMake/PolosTarget.cmake)
include(CMake/PolosApp.cmake)

add_compile_options(
    "$<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>"
    "$<$<C_COMPILER_ID:Clang>:-fansi-escape-codes>"
    "$<$<C_COMPILER_ID:GNU>:-fdiagnostics-color=always>"
)

add_compile_definitions(
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

if (BUILD_TESTS)
    message(STATUS "[POLOS] Building all tests")
    enable_testing()
    include(GoogleTest)
endif()

add_subdirectory(ThirdParty SYSTEM)
add_subdirectory(Source)
add_subdirectory(Tools)

add_custom_target(
    install-runtime
        COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" --component Runtime
        DEPENDS polos
        VERBATIM
)

add_custom_target(
    install-sdk
        COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" --component SDK
        DEPENDS install-runtime ResourcePreprocessor
        VERBATIM
)

if (BUILD_EXAMPLES)
    add_subdirectory(Examples)
    if (POLOS_EXAMPLE AND NOT TARGET "${POLOS_EXAMPLE}")
        message(FATAL_ERROR "Unknown Polos example: ${POLOS_EXAMPLE}")
    endif()
endif()

# The runtime and its examples share one entity capacity per build tree.
set(POLOS_ENTITY_CAPACITY 4194304)
if (BUILD_EXAMPLES AND POLOS_EXAMPLE)
    get_property(example_capacity TARGET "${POLOS_EXAMPLE}" PROPERTY POLOS_ENTITY_CAPACITY)
    if (DEFINED example_capacity)
        set(POLOS_ENTITY_CAPACITY "${example_capacity}")
    endif()
endif()
if (NOT POLOS_ENTITY_CAPACITY MATCHES "^[1-9][0-9]*$" OR POLOS_ENTITY_CAPACITY GREATER 4194304)
    message(FATAL_ERROR "ENTITY_CAPACITY must be an integer between 1 and 4194304")
endif()
configure_file(polos_config.hpp.in "${POLOS_CONFIG_INCLUDE_DIR}/polos/polos_config.hpp" @ONLY)

set_property(TARGET polos PROPERTY POLOS_ENTITY_CAPACITY "${POLOS_ENTITY_CAPACITY}")
set_property(TARGET polos APPEND PROPERTY EXPORT_PROPERTIES POLOS_ENTITY_CAPACITY)

include(CMakePackageConfigHelpers)
set(polos_cmake_dir "lib/cmake/Polos")
configure_package_config_file(
    CMake/PolosConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/PolosConfig.cmake"
    INSTALL_DESTINATION "${polos_cmake_dir}"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PolosConfigVersion.cmake"
    COMPATIBILITY ExactVersion
)
install(EXPORT PolosTargets
    NAMESPACE polos::
    DESTINATION "${polos_cmake_dir}"
    COMPONENT SDK
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PolosConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PolosConfigVersion.cmake"
    CMake/PolosApp.cmake
    DESTINATION "${polos_cmake_dir}"
    COMPONENT SDK
)

if (CMAKE_EXPORT_COMPILE_COMMANDS AND CMAKE_GENERATOR MATCHES "Ninja|Makefiles")
    add_custom_target(
        copy-compile-commands
            ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}"
            VERBATIM
    )

    foreach (target polos ResourcePreprocessor YamlReader)
        add_dependencies(
            ${target}
                copy-compile-commands
        )
    endforeach()
endif()
