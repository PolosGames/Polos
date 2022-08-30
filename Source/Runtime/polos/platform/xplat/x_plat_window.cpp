#include "polos/polos_pch.h"
#if defined(USE_OPENGL) || defined(USE_VULKAN)

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "polos/events/window/window_events.h"
#include "polos/events/input/input_events.h"
#include "polos/core/event_bus.h"
#include "polos/core/window_system.h"
#include "polos/core/window.h"

namespace polos
{
#pragma region callback_templates
    static void WindowCloseCallback(GLFWwindow* /*window*/)
    {
        EventBus::RaiseEvent<window_close>();
    }

    static void WindowFocusCallback(GLFWwindow* window, int32 focused)
    {
        if (window != glfwGetCurrentContext())
        {
            glfwMakeContextCurrent(window);
        }
        EventBus::RaiseEvent<window_focus>(focused);
    }

    static void WindowSizeCallback(GLFWwindow* window, int32 width, int32 height)
    {
        window_props* props = static_cast<window_props*>(glfwGetWindowUserPointer(window));
        props->width = width;
        props->height = height;
        EventBus::RaiseEvent<window_resize>(width, height);
    }

    static void WindowIconifyCallback(GLFWwindow* /*window*/, int32 iconified)
    {
        EventBus::RaiseEvent<window_iconify>(iconified);
    }

    static void WindowMaximizeCallback(GLFWwindow* /*window*/, int32 maximized)
    {
        EventBus::RaiseEvent<window_maximize>(maximized);
    }

    static void FramebufferSizeCallback(GLFWwindow* /*window*/, int32 width, int32 height)
    {
        EventBus::RaiseEvent<window_framebuffer_size>();
        glViewport(0, 0, width, height);
    }

    static void KeyCallback(
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

    static void CharCallback(GLFWwindow*, uint32 unicode)
    {
        EventBus::RaiseEvent<char_type>(unicode);
    }

    static void MouseButtonCallback(
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

    static void ScrollCallback(
        GLFWwindow* /*window*/,
        double x_offset,
        double y_offset
    )
    {
        EventBus::RaiseEvent<mouse_scroll>(float(x_offset), static_cast<float>(y_offset));
    }

    static void CursorPosCallback(GLFWwindow* /*window*/, double x, double y)
    {
        EventBus::RaiseEvent<mouse_move>(static_cast<float>(x), static_cast<float>(y));
    }
#pragma endregion

    void Window::Create()
    {
        GLFWmonitor* monitor = nullptr;
        GLFWvidmode const* mode = nullptr;
        if (props.fullscreen)
        {
            monitor = glfwGetPrimaryMonitor();
            mode = glfwGetVideoMode(monitor);

            props.width = mode->width;
            props.height = mode->height;
            props.refresh_rate = mode->refreshRate;
        }

        m_WinHandle = glfwCreateWindow(
            props.width,
            props.height,
            props.title.c_str(),
            monitor,
            nullptr
        );

        auto win_handle = static_cast<GLFWwindow*>(m_WinHandle);

        glfwSetWindowUserPointer(win_handle, &props);
        glfwMakeContextCurrent(win_handle);

        if (m_IsMainWindow)
        {
            if (std::shared_ptr gfx_ctx = m_GfxContext.lock())
            {
                gfx_ctx->Initialize(nullptr);
            }
        }

        glfwSwapInterval(props.vsync);

        glfwSetWindowCloseCallback(win_handle, WindowCloseCallback);
        glfwSetWindowFocusCallback(win_handle, WindowFocusCallback);
        glfwSetWindowSizeCallback(win_handle, WindowSizeCallback);
        glfwSetWindowIconifyCallback(win_handle, WindowIconifyCallback);
        glfwSetWindowMaximizeCallback(win_handle, WindowMaximizeCallback);
        glfwSetFramebufferSizeCallback(win_handle, FramebufferSizeCallback);

        glfwSetKeyCallback(win_handle, KeyCallback);
        glfwSetCharCallback(win_handle, CharCallback);

        glfwSetMouseButtonCallback(win_handle, MouseButtonCallback);
        glfwSetScrollCallback(win_handle, ScrollCallback);
        glfwSetCursorPosCallback(win_handle, CursorPosCallback);
    }

    void Window::Destroy()
    {
        glfwDestroyWindow(static_cast<GLFWwindow*>(m_WinHandle));
    }

    int32 Window::Width() const
    {
        return props.width;
    }

    int32 Window::Height() const
    {
        return props.height;
    }

    bool Window::Vsync() const
    {
        return props.vsync;
    }

    void Window::Vsync(bool vsync)
    {
        glfwSwapInterval(vsync);
        props.vsync = vsync;
    }

    void Window::Update() const
    {
        glfwPollEvents();
        glfwSwapBuffers(static_cast<GLFWwindow*>(m_WinHandle));
    }

    bool Window::IsOpen() const
    {
        return !glfwWindowShouldClose(static_cast<GLFWwindow*>(m_WinHandle));
    }
} // namespace polos

#endif