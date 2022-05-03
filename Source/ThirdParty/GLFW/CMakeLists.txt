cmake_minimum_required(VERSION 3.21)

add_library(glfw STATIC)

if(UNIX)
    set(GLFW_LIB_NAME glfw)
else()
    set(GLFW_LIB_NAME glfw3)
endif()

set(GLFW_VERSION_MAJOR 3)
set(GLFW_VERSION_MINOR 3)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(_GLFW_WIN32 1)
    target_compile_definitions(glfw PRIVATE _GLFW_WIN32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_GLFW_X11 1)
    target_compile_definitions(glfw PRIVATE _GLFW_X11)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin") # apple
    set(_GLFW_COCOA 1)
    target_compile_definitions(glfw PRIVATE _GLFW_COCOA)
endif()

set(GLFW_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(GLFW_BINARY_DIR ${CMAKE_BINARY_DIR}/ThirdParty/GLFW)

set(glfw_HEADERS
    ${GLFW_SOURCE_DIR}/src/internal.h
    ${GLFW_SOURCE_DIR}/src/mappings.h
    "${GLFW_SOURCE_DIR}/include/GLFW/glfw3.h"
    "${GLFW_SOURCE_DIR}/include/GLFW/glfw3native.h"
    )
set(glfw_SOURCES
    ${GLFW_SOURCE_DIR}/src/context.c
    ${GLFW_SOURCE_DIR}/src/init.c
    ${GLFW_SOURCE_DIR}/src/input.c
    ${GLFW_SOURCE_DIR}/src/monitor.c
    ${GLFW_SOURCE_DIR}/src/vulkan.c
    ${GLFW_SOURCE_DIR}/src/window.c
    )

if (_GLFW_COCOA)
    list(APPEND glfw_HEADERS
        ${GLFW_SOURCE_DIR}/src/cocoa_platform.h
        ${GLFW_SOURCE_DIR}/src/cocoa_joystick.h
        ${GLFW_SOURCE_DIR}/src/posix_thread.h
        ${GLFW_SOURCE_DIR}/src/nsgl_context.h
        ${GLFW_SOURCE_DIR}/src/egl_context.h
        ${GLFW_SOURCE_DIR}/src/osmesa_context.h)
    
    list(APPEND glfw_SOURCES
        ${GLFW_SOURCE_DIR}/src/cocoa_init.m
        ${GLFW_SOURCE_DIR}/src/cocoa_joystick.m
        ${GLFW_SOURCE_DIR}/src/cocoa_monitor.m
        ${GLFW_SOURCE_DIR}/src/cocoa_window.m
        ${GLFW_SOURCE_DIR}/src/cocoa_time.c
        ${GLFW_SOURCE_DIR}/src/posix_thread.c
        ${GLFW_SOURCE_DIR}/src/nsgl_context.m
        ${GLFW_SOURCE_DIR}/src/egl_context.c
        ${GLFW_SOURCE_DIR}/src/osmesa_context.c)
elseif (_GLFW_WIN32)
    list(APPEND glfw_HEADERS
        ${GLFW_SOURCE_DIR}/src/win32_platform.h
        ${GLFW_SOURCE_DIR}/src/win32_joystick.h
        ${GLFW_SOURCE_DIR}/src/wgl_context.h
        ${GLFW_SOURCE_DIR}/src/egl_context.h
        ${GLFW_SOURCE_DIR}/src/osmesa_context.h)
    
    list(APPEND glfw_SOURCES
        ${GLFW_SOURCE_DIR}/src/win32_init.c
        ${GLFW_SOURCE_DIR}/src/win32_joystick.c
        ${GLFW_SOURCE_DIR}/src/win32_monitor.c
        ${GLFW_SOURCE_DIR}/src/win32_time.c
        ${GLFW_SOURCE_DIR}/src/win32_thread.c
        ${GLFW_SOURCE_DIR}/src/win32_window.c
        ${GLFW_SOURCE_DIR}/src/wgl_context.c
        ${GLFW_SOURCE_DIR}/src/egl_context.c
        ${GLFW_SOURCE_DIR}/src/osmesa_context.c)
elseif (_GLFW_X11)
    list(APPEND glfw_HEADERS
        ${GLFW_SOURCE_DIR}/src/x11_platform.h
        ${GLFW_SOURCE_DIR}/src/xkb_unicode.h
        ${GLFW_SOURCE_DIR}/src/posix_time.h
        ${GLFW_SOURCE_DIR}/src/posix_thread.h
        ${GLFW_SOURCE_DIR}/src/glx_context.h
        ${GLFW_SOURCE_DIR}/src/egl_context.h
        ${GLFW_SOURCE_DIR}/src/osmesa_context.h)
    list(APPEND glfw_SOURCES
        ${GLFW_SOURCE_DIR}/src/x11_init.c
        ${GLFW_SOURCE_DIR}/src/x11_monitor.c
        ${GLFW_SOURCE_DIR}/src/x11_window.c
        ${GLFW_SOURCE_DIR}/src/xkb_unicode.c
        ${GLFW_SOURCE_DIR}/src/posix_time.c
        ${GLFW_SOURCE_DIR}/src/posix_thread.c
        ${GLFW_SOURCE_DIR}/src/glx_context.c
        ${GLFW_SOURCE_DIR}/src/egl_context.c
        ${GLFW_SOURCE_DIR}/src/osmesa_context.c)
endif()

if (${_GLFW_X11})
    find_package(X11 REQUIRED)
    target_include_directories(glfw PRIVATE "${X11_X11_INCLUDE_PATH}")
    
    # Check for XRandR (modern resolution switching and gamma control)
    if (NOT X11_Xrandr_INCLUDE_PATH)
        message(FATAL_ERROR "RandR headers not found; install libxrandr development package")
    endif()
    target_include_directories(glfw PRIVATE "${X11_Xrandr_INCLUDE_PATH}")
    
    # Check for Xinerama (legacy multi-monitor support)
    if (NOT X11_Xinerama_INCLUDE_PATH)
        message(FATAL_ERROR "Xinerama headers not found; install libxinerama development package")
    endif()
    target_include_directories(glfw PRIVATE "${X11_Xinerama_INCLUDE_PATH}")
    
    # Check for Xkb (X keyboard extension)
    if (NOT X11_Xkb_INCLUDE_PATH)
        message(FATAL_ERROR "XKB headers not found; install X11 development package")
    endif()
    target_include_directories(glfw PRIVATE "${X11_Xkb_INCLUDE_PATH}")
    
    # Check for Xcursor (cursor creation from RGBA images)
    if (NOT X11_Xcursor_INCLUDE_PATH)
        message(FATAL_ERROR "Xcursor headers not found; install libxcursor development package")
    endif()
    target_include_directories(glfw PRIVATE "${X11_Xcursor_INCLUDE_PATH}")
    
    # Check for XInput (modern HID input)
    if (NOT X11_Xi_INCLUDE_PATH)
        message(FATAL_ERROR "XInput headers not found; install libxi development package")
    endif()
    target_include_directories(glfw PRIVATE "${X11_Xi_INCLUDE_PATH}")
    
    # Check for X Shape (custom window input shape)
    if (NOT X11_Xshape_INCLUDE_PATH)
        message(FATAL_ERROR "X Shape headers not found; install libxext development package")
    endif()
    target_include_directories(glfw PRIVATE "${X11_Xshape_INCLUDE_PATH}")

    target_link_libraries(glfw PRIVATE "${X11_LIBRARIES}")
endif()

if (${_GLFW_X11})
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND glfw_HEADERS ${GLFW_SOURCE_DIR}/src/linux_joystick.h)
        list(APPEND glfw_SOURCES ${GLFW_SOURCE_DIR}/src/linux_joystick.c)
    else()
        list(APPEND glfw_HEADERS ${GLFW_SOURCE_DIR}/src/null_joystick.h)
        list(APPEND glfw_SOURCES ${GLFW_SOURCE_DIR}/src/null_joystick.c)
    endif()
endif()

target_sources(glfw PRIVATE ${glfw_SOURCES} ${glfw_HEADERS})

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
    set_source_files_properties(
    ${GLFW_SOURCE_DIR}/src/context.c
    ${GLFW_SOURCE_DIR}/src/init.c
    ${GLFW_SOURCE_DIR}/src/input.c
    ${GLFW_SOURCE_DIR}/src/monitor.c
    ${GLFW_SOURCE_DIR}/src/vulkan.c
    ${GLFW_SOURCE_DIR}/src/i_window.c
    ${GLFW_SOURCE_DIR}/src/win32_init.c
    ${GLFW_SOURCE_DIR}/src/win32_joystick.c
    ${GLFW_SOURCE_DIR}/src/win32_monitor.c
    ${GLFW_SOURCE_DIR}/src/win32_time.c
    ${GLFW_SOURCE_DIR}/src/win32_thread.c
    ${GLFW_SOURCE_DIR}/src/win32_window.c
    ${GLFW_SOURCE_DIR}/src/wgl_context.c
    ${GLFW_SOURCE_DIR}/src/egl_context.c
    ${GLFW_SOURCE_DIR}/src/osmesa_context.c
    PROPERTIES COMPILE_FLAGS -Wdeclaration-after-statement)

endif()

if (UNIX AND NOT APPLE)
    find_library(RT_LIBRARY rt)
    mark_as_advanced(RT_LIBRARY)
    if (RT_LIBRARY)
        target_link_libraries(glfw PRIVATE "${RT_LIBRARY}")
        list(APPEND glfw_PKG_LIBS "-lrt")
    endif()

    find_library(MATH_LIBRARY m)
    mark_as_advanced(MATH_LIBRARY)
    if (MATH_LIBRARY)
        target_link_libraries(glfw PRIVATE "${MATH_LIBRARY}")
        list(APPEND glfw_PKG_LIBS "-lm")
    endif()

    if (CMAKE_DL_LIBS)
        target_link_libraries(glfw PRIVATE "${CMAKE_DL_LIBS}")
        list(APPEND glfw_PKG_LIBS "-l${CMAKE_DL_LIBS}")
    endif()
endif()

build_options(glfw false)

if (_GLFW_WIN32)
    target_compile_definitions(glfw PRIVATE UNICODE _UNICODE)
endif()

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    target_compile_definitions(glfw PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

#install(TARGETS glfw DESTINATION bin)
#install(FILES include/GLFW/glfw3.h include/GLFW/glfw3native.h DESTINATION include/ThirdParty/GLFW)