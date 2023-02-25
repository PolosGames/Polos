#include "polos/polos_pch.h"

#include <glad/glad.h>

#include "polos/platform/xplat/x_plat_window_callbacks.h"
#include "polos/core/event_bus.h"
#include "polos/events/events.h"
#include "polos/core/window.h"
#include "polos/core/window_system.h"

namespace polos
{
    void GLFWErrorCallback(int p_ErrorCode, const char* p_Description)
    {
        switch (p_ErrorCode)
        {
            case GLFW_INVALID_ENUM:
                LOG_ENGINE_WARN("GLFW received an invalid enum to it's function! Desc: {0}", p_Description);
                break;
            case GLFW_INVALID_VALUE:
                LOG_ENGINE_WARN("GLFW received an invalid value to it's function! Desc: {0}", p_Description);
                break;
            case GLFW_OUT_OF_MEMORY:
                LOG_ENGINE_CRITICAL("A memory allocation failed within GLFW or the operating system! Desc: {0}", p_Description);
                break;
            case GLFW_API_UNAVAILABLE:
                LOG_ENGINE_ERROR("GLFW could not find support for the requested API on the system! Desc: {0}", p_Description);
                break;
            case GLFW_FORMAT_UNAVAILABLE:
                LOG_ENGINE_ERROR("The requested pixel format is not supported! Desc: {0}", p_Description);
                break;
        }
    }

    void WindowCloseCallback(GLFWwindow* p_Window)
    {
        // Raise the event with the window handle so we know which window to
        // close.
        EventBus::RaiseEvent<window_close>(p_Window);
    }

    void WindowFocusCallback(GLFWwindow* p_Window, int32 p_Focused)
    {
        if (p_Window != glfwGetCurrentContext())
        {
            glfwMakeContextCurrent(p_Window);
        }
        EventBus::RaiseEvent<window_focus>(p_Focused);
    }

    void WindowSizeCallback(GLFWwindow* p_Window, int32 p_Width, int32 p_Height)
    {
        window_props* props = static_cast<window_props*>(glfwGetWindowUserPointer(p_Window));
        props->width        = p_Width;
        props->height       = p_Height;
        EventBus::RaiseEvent<window_resize>(p_Width, p_Height);
    }
    
    void WindowIconifyCallback(GLFWwindow* /*p_Window*/, int32 p_Iconified)
    {
        EventBus::RaiseEvent<window_iconify>(p_Iconified);
    }

    void WindowMaximizeCallback(GLFWwindow* /*p_Window*/, int32 p_Maximized)
    {
        EventBus::RaiseEvent<window_maximize>(p_Maximized);
    }

    void FramebufferSizeCallback(GLFWwindow* /*p_Window*/, int32 p_Width, int32 p_Height)
    {
        EventBus::RaiseEvent<window_framebuffer_size>();
        glViewport(0, 0, p_Width, p_Height);
    }

    void KeyCallback(
        GLFWwindow* /*p_Window*/,
        int32 p_Key,
        int32 /*p_Scancode*/,
        int32 p_Action,
        int32 /*p_Mods*/
    )
    {
        switch (p_Action)
        {
            case GLFW_PRESS:
                EventBus::RaiseEvent<key_press>(p_Key);
                break;
            case GLFW_RELEASE:
                EventBus::RaiseEvent<key_release>(p_Key);
                break;
        }
    }

    void CharCallback(GLFWwindow* /*p_Window*/, uint32 p_Unicode)
    {
        EventBus::RaiseEvent<char_type>(p_Unicode);
    }

    void MouseButtonCallback(
        GLFWwindow* /*p_Window*/,
        int32 p_Button,
        int32 p_Action,
        int32 /*p_Mods*/
    )
    {
        switch (p_Action)
        {
            case GLFW_PRESS:
                EventBus::RaiseEvent<mouse_button_press>(p_Button);
                break;
            case GLFW_RELEASE:
                EventBus::RaiseEvent<mouse_button_release>(p_Button);
                break;
        }
    }

    void ScrollCallback(
        GLFWwindow* /*p_Window*/,
        double p_XOffset,
        double p_YOffset
    )
    {
        EventBus::RaiseEvent<mouse_scroll>(static_cast<float>(p_XOffset), static_cast<float>(p_YOffset));
    }

    void CursorPosCallback(GLFWwindow* /*p_Window*/, double p_XOffset, double p_YOffset)
    {
        EventBus::RaiseEvent<mouse_move>(static_cast<float>(p_XOffset), static_cast<float>(p_YOffset));
    }
} // namespace polos