set(glfw_HEADERS
        src/internal.h
        src/mappings.h
        "${GLFW_BINARY_DIR}/src/glfw_config.h"
        "${GLFW_SOURCE_DIR}/include/GLFW/glfw3.h"
        "${GLFW_SOURCE_DIR}/include/GLFW/glfw3native.h"
        )
set(glfw_SOURCES
	src/context.c
	src/init.c
	src/input.c
	src/monitor.c
	src/vulkan.c
	src/window.c
    )

if (_GLFW_COCOA)
    list(APPEND glfw_HEADERS
        src/cocoa_platform.h
        src/cocoa_joystick.h
        src/posix_thread.h
        src/nsgl_context.h
        src/egl_context.h
        src/osmesa_context.h)

    list(APPEND glfw_SOURCES
        src/cocoa_init.m
        src/cocoa_joystick.m
        src/cocoa_monitor.m
        src/cocoa_window.m
        src/cocoa_time.c
        src/posix_thread.c
        nsgl_context.m
        egl_context.c
        osmesa_context.c)
elseif (_GLFW_WIN32)
    list(APPEND glfw_HEADERS
        src/win32_platform.h
        src/win32_joystick.h
        src/wgl_context.h
        src/egl_context.h
        src/osmesa_context.h)

    list(APPEND glfw_SOURCES
        src/win32_init.c
        src/win32_joystick.c
        src/win32_monitor.c
        src/win32_time.c
        src/win32_thread.c
        src/win32_window.c
        src/wgl_context.c
        src/egl_context.c
        src/osmesa_context.c)
elseif (_GLFW_X11)
    list(APPEND glfw_HEADERS
        src/x11_platform.h
        src/xkb_unicode.h
        src/posix_time.h
        src/posix_thread.h
        src/glx_context.h
        src/egl_context.h
        src/osmesa_context.h)
    list(APPEND glfw_SOURCES
        src/x11_init.c
        src/x11_monitor.c
        src/x11_window.c
        src/xkb_unicode.c
        src/posix_time.c
        src/posix_thread.c
        src/glx_context.c
        src/egl_context.c
        src/osmesa_context.c)
endif()

if (_GLFW_X11 OR _GLFW_WAYLAND)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND glfw_HEADERS linux_joystick.h)
        list(APPEND glfw_SOURCES linux_joystick.c)
    else()
        list(APPEND glfw_HEADERS null_joystick.h)
        list(APPEND glfw_SOURCES null_joystick.c)
    endif()
endif()


