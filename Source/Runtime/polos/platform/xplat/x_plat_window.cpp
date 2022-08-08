#include "polos/polos_pch.h"
#if defined(USE_OPENGL) || defined(USE_VULKAN)

#include "GLFW/glfw3.h"

#include "polos/events/window/window_events.h"
#include "polos/events/input/input_events.h"
#include "polos/core/event_bus.h"
#include "polos/core/window_system.h"

#include "x_plat_window.h"

namespace polos
{
    bool   XPlatWindow::m_IsInitialized = false;
    uint32 XPlatWindow::m_WindowCount = 0;
    
    SharedPtr<IWindow> WindowSystem::NewWindow()
    {
        return m_Instance->m_Windows.emplace_back(new XPlatWindow());
    }

    static void error_callback(int error_code, const char* description)
    {
        switch (error_code)
        {
        case GLFW_INVALID_ENUM: LOG_ENGINE_WARN("GLFW received an invalid enum to it's function! Desc: {0}", description); break;
        case GLFW_INVALID_VALUE: LOG_ENGINE_WARN("GLFW received an invalid value to it's function! Desc: {0}", description); break;
        case GLFW_OUT_OF_MEMORY: LOG_ENGINE_CRITICAL("A memory allocation failed within GLFW or the operating system! Desc: {0}", description); break;
        case GLFW_API_UNAVAILABLE: LOG_ENGINE_ERROR("GLFW could not find support for the requested API on the system! Desc: {0}", description); break;
        case GLFW_FORMAT_UNAVAILABLE: LOG_ENGINE_ERROR("The requested pixel format is not supported! Desc: {0}", description); break;
        }
    }

    void XPlatWindow::Shutdown()
    {
        glfwTerminate();
    }

    void XPlatWindow::Initialize()
    {
        if (!m_IsInitialized)
        {
            glfwSetErrorCallback(error_callback);
            int r = glfwInit(); static_cast<void>(r);
            ASSERTSTR(r == GLFW_TRUE, "Failed to initialize GLFW!");
            m_IsInitialized = true;
        }

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

        m_Window = glfwCreateWindow(props.width, props.height, props.title.c_str(), monitor, nullptr);
        glfwSetWindowUserPointer(m_Window, &props);
        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(props.vsync);

        m_Context = std::make_unique<graphics_context>();
        m_Context->Initialize(m_Window);
        glViewport(0, 0, props.width, props.height);

#pragma region window_events

        glfwSetWindowCloseCallback(m_Window,
            [](GLFWwindow* /*window*/)
            {
                EventBus::RaiseEvent<window_close>();
            }
        );

        glfwSetWindowFocusCallback(m_Window,
            [](GLFWwindow* /*window*/, int32 focused)
            {
                EventBus::RaiseEvent<window_focus>(focused);
            }
        );

        glfwSetWindowSizeCallback(m_Window,
            [](GLFWwindow* window, int32 width, int32 height)
            {
                window_props* props = static_cast<window_props*>(glfwGetWindowUserPointer(window));
                props->width = width;
                props->height = height;
                EventBus::RaiseEvent<window_resize>(width, height);
            }
        );

        glfwSetWindowIconifyCallback(m_Window,
            [](GLFWwindow* /*window*/, int32 iconified)
            {
                EventBus::RaiseEvent<window_iconify>(iconified);
            }
        );

        glfwSetWindowMaximizeCallback(m_Window,
            [](GLFWwindow* /*window*/, int32 maximized)
            {
                EventBus::RaiseEvent<window_maximize>(maximized);
            }
        );

        glfwSetFramebufferSizeCallback(m_Window,
            [](GLFWwindow* /*window*/, int32 width, int32 height)
            {
                EventBus::RaiseEvent<window_framebuffer_size>();
                glViewport(0, 0, width, height);
            }
        );

#pragma endregion

#pragma region key_input_events

        glfwSetKeyCallback(m_Window,
            [](GLFWwindow* /*window*/, int32 key, int32 /*scancode*/, int32 action, int32 /*mods*/)
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
        );

        glfwSetCharCallback(m_Window,
            [](GLFWwindow*, uint32 unicode)
            {
                EventBus::RaiseEvent<char_type>(unicode);
            }
        );

#pragma endregion

#pragma region mouse_input_events

        glfwSetMouseButtonCallback(m_Window,
            [](GLFWwindow* /*window*/, int32 button, int32 action, int32 /*mods*/) {
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
        );

        glfwSetScrollCallback(m_Window,
            [](GLFWwindow* /*window*/, double x_offset, double y_offset)
            {
                EventBus::RaiseEvent<mouse_scroll>(float(x_offset), static_cast<float>(y_offset));
            }
        );

        glfwSetCursorPosCallback(m_Window,
            [](GLFWwindow* /*window*/, double x, double y)
            {
                EventBus::RaiseEvent<mouse_move>(static_cast<float>(x), static_cast<float>(y));
            }
        );

#pragma endregion
    }

    void XPlatWindow::Destroy()
    {
        glfwDestroyWindow(m_Window);
    }

    int32 XPlatWindow::Width() const
    {
        return props.width;
    }

    int32 XPlatWindow::Height() const
    {
        return props.height;
    }

    bool XPlatWindow::Vsync() const
    {
        return props.vsync;
    }

    void XPlatWindow::Vsync(bool vsync)
    {
        glfwSwapInterval(vsync);
        props.vsync = vsync;
    }

    void XPlatWindow::Update() const
    {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    bool XPlatWindow::IsOpen() const
    {
        return !glfwWindowShouldClose(m_Window);
    }
} // namespace polos

#endif