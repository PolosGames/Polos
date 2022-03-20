cmake_minimum_required(VERSION 3.21)

if(UNIX)
    set(GLFW_LIB_NAME glfw)
else()
    set(GLFW_LIB_NAME glfw3)
endif()

set(GLFW_VERSION_MAJOR 3)
set(GLFW_VERSION_MINOR 3)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(_GLFW_WIN32 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_GLFW_X11 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin") # apple
    set(_GLFW_COCOA 1)
endif()

set(GLFW_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(GLFW_BINARY_DIR ${CMAKE_BINARY_DIR}/ThirdParty/GLFW)

configure_file(src/glfw_config.h.in src/glfw_config.h @ONLY)

# pull the sources'n stuff
include(src/CMakeLists.txt)

add_library(glfw STATIC ${glfw_SOURCES} ${glfw_HEADERS})

set_target_properties(glfw PROPERTIES C_STANDARD 99)

target_compile_definitions(glfw PRIVATE _GLFW_USE_CONFIG_H)
target_include_directories(glfw PUBLIC
                           "$<BUILD_INTERFACE:${GLFW_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_include_directories(glfw PRIVATE
                           "${GLFW_SOURCE_DIR}/src"
                           "${GLFW_BINARY_DIR}/src")

# Make GCC warn about declarations that VS 2010 and 2012 won't accept for all
# source files that VS will build (Clang ignores this because we set -std=c99)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set_source_files_properties(src/context.c src/init.c src/input.c src/monitor.c src/vulkan.c
                                src/i_window.c src/win32_init.c src/win32_joystick.c
            src/win32_monitor.c src/win32_time.c src/win32_thread.c
            src/win32_window.c src/wgl_context.c src/egl_context.c
            src/osmesa_context.c PROPERTIES
                                COMPILE_FLAGS -Wdeclaration-after-statement)

endif()

build_options(glfw)

if (_GLFW_WIN32)
    target_compile_definitions(glfw PRIVATE UNICODE _UNICODE)
endif()

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    target_compile_definitions(glfw PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

#install(TARGETS glfw DESTINATION bin)
#install(FILES include/GLFW/glfw3.h include/GLFW/glfw3native.h DESTINATION include/ThirdParty/GLFW)