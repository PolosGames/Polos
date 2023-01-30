#include "polos/polos_pch.h"

#include <glad/glad.h>

#include "polos/platform/xplat/x_plat_window_callbacks.h"
#include "polos/core/event_bus.h"
#include "polos/events/events.h"
#include "polos/core/window.h"

namespace polos
{
    void GLFWErrorCallback(int error_code, const char* description)
    {
        switch (error_code)
        {
            case GLFW_INVALID_ENUM:
                LOG_ENGINE_WARN("GLFW received an invalid enum to it's function! Desc: {0}", description);
                break;
            case GLFW_INVALID_VALUE:
                LOG_ENGINE_WARN("GLFW received an invalid value to it's function! Desc: {0}", description);
                break;
            case GLFW_OUT_OF_MEMORY:
                LOG_ENGINE_CRITICAL("A memory allocation failed within GLFW or the operating system! Desc: {0}", description);
                break;
            case GLFW_API_UNAVAILABLE:
                LOG_ENGINE_ERROR("GLFW could not find support for the requested API on the system! Desc: {0}", description);
                break;
            case GLFW_FORMAT_UNAVAILABLE:
                LOG_ENGINE_ERROR("The requested pixel format is not supported! Desc: {0}", description);
                break;
        }
    }

    void WindowCloseCallback(GLFWwindow* window)
    {
        // Raise the event with the window handle so we know which window to
        // close.
        EventBus::RaiseEvent<window_close>(window);
    }

    void WindowFocusCallback(GLFWwindow* window, int32 focused)
    {
        if (window != glfwGetCurrentContext())
        {
            glfwMakeContextCurrent(window);
        }
        EventBus::RaiseEvent<window_focus>(focused);
    }

    void WindowSizeCallback(GLFWwindow* window, int32 width, int32 height)
    {
        window_props* props = static_cast<window_props*>(glfwGetWindowUserPointer(window));
        props->width        = width;
        props->height       = height;
        EventBus::RaiseEvent<window_resize>(width, height);
    }
    
    void WindowIconifyCallback(GLFWwindow* /*window*/, int32 iconified)
    {
        EventBus::RaiseEvent<window_iconify>(iconified);
    }

    void WindowMaximizeCallback(GLFWwindow* /*window*/, int32 maximized)
    {
        EventBus::RaiseEvent<window_maximize>(maximized);
    }

    void FramebufferSizeCallback(GLFWwindow* /*window*/, int32 width, int32 height)
    {
        EventBus::RaiseEvent<window_framebuffer_size>();

        glViewport(0, 0, width, height);
    }

    void KeyCallback(
        GLFWwindow* /*window*/,
        int32 key,
        int32 /*scancode*/,
        int32 action,
        int32 /*mods*/
    )
    {
        switch (action)
        {
            case GLFW_PRESS:
                EventBus::RaiseEvent<key_press>(key);
                break;
            case GLFW_RELEASE:
                EventBus::RaiseEvent<key_release>(key);
                break;
        }
    }

    void CharCallback(GLFWwindow*, uint32 unicode)
    {
        EventBus::RaiseEvent<char_type>(unicode);
    }

    void MouseButtonCallback(
        GLFWwindow* /*window*/,
        int32 button,
        int32 action,
        int32 /*mods*/
    )
    {
        switch (action)
        {
            case GLFW_PRESS:
                EventBus::RaiseEvent<mouse_button_press>(button);
                break;
            case GLFW_RELEASE:
                EventBus::RaiseEvent<mouse_button_release>(button);
                break;
        }
    }

    void ScrollCallback(
        GLFWwindow* /*window*/,
        double x_offset,
        double y_offset
    )
    {
        EventBus::RaiseEvent<mouse_scroll>(float(x_offset), static_cast<float>(y_offset));
    }

    void CursorPosCallback(GLFWwindow* /*window*/, double x, double y)
    {
        EventBus::RaiseEvent<mouse_move>(static_cast<float>(x), static_cast<float>(y));
    }
} // namespace polos