cmake_minimum_required(VERSION 3.24)

add_compile_definitions(
    GLFW_INCLUDE_NONE
    _CRT_SECURE_NO_WARNINGS
)

# Runtime library should be used for linking to the application itself.

add_library(Runtime INTERFACE)

set_target_properties(
    Runtime
        PROPERTIES
            OUTPUT_NAME "PolosRuntime"
            CXX_STANDARD ${POLOS_CPP_STANDARD}
            CXX_STANDARD_REQUIRED True
)

add_subdirectory(polos/containers)
add_subdirectory(polos/core)
add_subdirectory(polos/graphics)
add_subdirectory(polos/gui)
add_subdirectory(polos/platform)
add_subdirectory(polos/utils)

target_link_libraries(
    Runtime
        INTERFACE
            PolosCore
            PolosContainers
            PolosGraphics
            PolosGUI
            PolosPlatform
            PolosUtils
)

target_include_directories(
    Runtime
        INTERFACE
            ${POLOS_INCLUDE_DIR}
)
