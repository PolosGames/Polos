cmake_minimum_required(VERSION 3.25)
if (POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
endif()
# The LINUX platform variable used by the build requires CMake 3.25.

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" POLOS_VERSION)
string(STRIP "${POLOS_VERSION}" POLOS_VERSION)
if (NOT POLOS_VERSION MATCHES "^(0|[1-9][0-9]*)\\.(0|[1-9][0-9]*)\\.(0|[1-9][0-9]*)$")
    message(FATAL_ERROR "VERSION must contain a major.minor.patch version")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")

project(Polos
        VERSION ${POLOS_VERSION}
        LANGUAGES CXX C
        DESCRIPTION "Game engine for PolosGames written in C++23")

set(CMAKE_CXX_EXTENSIONS OFF)
include(GNUInstallDirs)

set(POLOS_CXX_STANDARD 23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(POLOS_BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}")
set(POLOS_DIR         "${CMAKE_CURRENT_LIST_DIR}/Source")
set(POLOS_EXAMPLES    "${CMAKE_CURRENT_LIST_DIR}/Examples")
if (NOT POLOS_INSTALL_DIR)
    set(POLOS_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
endif()
string(REPLACE "\\" "/" POLOS_INSTALL_DIR ${POLOS_INSTALL_DIR})

option(ENABLE_PROFILING        "Enable Tracy instrumentation" OFF)
option(GUI_USE_IMGUI           "Use ImGui for default Gui implementation" ON)
option(BUILD_EXAMPLES          "Will build all example projects under Examples directory." ON)
option(BUILD_TESTS             "Build all unit and component tests for the runtime" OFF)
option(BUILD_INTEGRATION_TESTS "Build window/Vulkan integration tests (requires BUILD_TESTS)" OFF)
option(ENABLE_CLANG_TIDY       "Enable clang-tidy static analysis during builds" OFF)

set(POLOS_INSTALL_INC_DIR "${POLOS_INSTALL_DIR}/include")
set(POLOS_INSTALL_LIB_DIR "${POLOS_INSTALL_DIR}")

message(STATUS "[POLOS] ENABLE_PROFILING: ${ENABLE_PROFILING}")
message(STATUS "[POLOS] GUI_USE_IMGUI: ${GUI_USE_IMGUI}")
message(STATUS "[POLOS] BUILD_TESTS: ${BUILD_TESTS}")
message(STATUS "[POLOS] POLOS_INSTALL_DIR: ${POLOS_INSTALL_DIR}")
message(STATUS "[POLOS] ENABLE_CLANG_TIDY: ${ENABLE_CLANG_TIDY}")

if (ENABLE_CLANG_TIDY)
    include(CMake/Tidy.cmake)
endif()

set(POLOS_CONFIG_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated/include")
configure_file(polos_config.hpp.in "${POLOS_CONFIG_INCLUDE_DIR}/polos/polos_config.hpp" @ONLY)
configure_file(polos_api.hpp.in "${POLOS_CONFIG_INCLUDE_DIR}/polos/polos_api.hpp" COPYONLY)

include(CMake/BuildOptions.cmake)
include(CMake/PolosTarget.cmake)

add_compile_options(
        "$<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>"
        "$<$<C_COMPILER_ID:Clang>:-fansi-escape-codes>"
        "$<$<C_COMPILER_ID:GNU>:-fdiagnostics-color=always>"
)

add_compile_definitions(
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    SIMDJSON_DEVELOPMENT_CHECKS
)

if (BUILD_TESTS)
    message(STATUS "[POLOS] Building all tests")
    find_package(GTest CONFIG REQUIRED)
    enable_testing()
endif()

find_package(ctre CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ryml CONFIG REQUIRED)
find_package(Vulkan 1.3 REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

add_subdirectory(ThirdParty SYSTEM)
add_subdirectory(Source)
add_subdirectory(Tools)

add_custom_target(install-runtime
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" --component Runtime
    DEPENDS polos
    VERBATIM
)

add_custom_target(install-sdk
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" --component SDK
    DEPENDS install-runtime
    VERBATIM
)

if (BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif ()

add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_LIST_DIR}
)

add_dependencies(ResourcePreprocessor copy-compile-commands)
add_dependencies(polos copy-compile-commands)
add_dependencies(YamlReader copy-compile-commands)
