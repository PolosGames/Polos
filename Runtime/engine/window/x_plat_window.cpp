#include "plpch.h"

#include "x_plat_window.h"
#if defined(USE_OPENGL) || defined(USE_VULKAN)

#include "debug.h"
#include "events/window/window_events.h"
#include "events/input/input_events.h"

namespace polos
{
#if defined(USE_OPENGL) || defined(USE_VULKAN)
    Window* Window::NewWindow(const window_props &props)
    {
        return new XPlatWindow(props);
    }
#endif
    bool XPlatWindow::_is_glfw_initialized = false;
    uint32 XPlatWindow::_glfw_window_count = 0;

    static void error_callback(int error_code, const char *description)
    {
        switch (error_code)
        {
        case GLFW_INVALID_ENUM: LOG_CORE_WARN("GLFW received an invalid enum to it's function! Desc: {0}", description); break;
        case GLFW_INVALID_VALUE: LOG_CORE_WARN("GLFW received an invalid value to it's function! Desc: {0}", description); break;
        case GLFW_OUT_OF_MEMORY: LOG_CORE_CRITICAL("A memory allocation failed within GLFW or the operating system! Desc: {0}", description); break;
        case GLFW_API_UNAVAILABLE: LOG_CORE_ERROR("GLFW could not find support for the requested API on the system! Desc: {0}", description); break;
        case GLFW_FORMAT_UNAVAILABLE: LOG_CORE_ERROR("The requested pixel format is not supported! Desc: {0}", description); break;
        }
    }

    void XPlatWindow::Shutdown()
    {
        glfwTerminate();
    }

    XPlatWindow::XPlatWindow(const window_props &props)
        : _props(props)
    {
        Initialize();
    }

    void XPlatWindow::Initialize()
    {
        if (!_is_glfw_initialized)
        {
            glfwSetErrorCallback(error_callback);
            int r = glfwInit();
            ASSERT_S(r == GLFW_TRUE, "Failed to initialize GLFW!");
            _is_glfw_initialized = true;
        }

        GLFWmonitor *monitor = nullptr;
        const GLFWvidmode *mode = nullptr;
        if (_props.fullscreen)
        {
            monitor = glfwGetPrimaryMonitor();
            mode = glfwGetVideoMode(monitor);

            _props.width = mode->width;
            _props.height = mode->height;
            _props.refresh_rate = mode->refreshRate;
        }

        window = glfwCreateWindow(_props.width, _props.height, _props.title.c_str(), monitor, nullptr);
        glfwSetWindowUserPointer(window, &_props);
        glfwMakeContextCurrent(window);

        _context = std::make_unique<graphics_context>();
        _context->Initialize(window);

    #pragma region window_events

        glfwSetWindowCloseCallback(window, 
            [](GLFWwindow *_)
            {
                event_bus::raise_event<window_close>();
            }
        );

        glfwSetWindowFocusCallback(window,
            [](GLFWwindow *_, int32 focused)
            {
                event_bus::raise_event<window_focus>(focused);
            }
        );

        glfwSetWindowSizeCallback(window,
            [](GLFWwindow *window, int32 width, int32 height)
            {
                window_props *props = static_cast<window_props*>(glfwGetWindowUserPointer(window));
                props->width = width;
                props->height = height;
                event_bus::raise_event<window_resize>(width, height);
            }
        );

        glfwSetWindowIconifyCallback(window,
            [](GLFWwindow *_, int32 iconified)
            {
                event_bus::raise_event<window_iconify>(iconified);
            }
        );

        glfwSetWindowMaximizeCallback(window,
            [](GLFWwindow *_, int32 maximized)
            {
                event_bus::raise_event<window_maximize>(maximized);
            }
        );

        glfwSetFramebufferSizeCallback(window,
            [](GLFWwindow *_, int32 width, int32 height)
            {
                event_bus::raise_event<window_framebuffer_size>();
            }
        );

    #pragma endregion

    #pragma region key_input_events

        glfwSetKeyCallback(window, 
            [](GLFWwindow *_, int32 key, int32 scancode, int32 action, int32 mods)
            {
                switch (action)
                {
                case GLFW_PRESS:
                    event_bus::raise_event<key_press>(key);
                    break;
                case GLFW_RELEASE:
                    event_bus::raise_event<key_release>(key);
                    break;
                }
            }
        );

    #pragma endregion

    #pragma region mouse_input_events

        glfwSetMouseButtonCallback(window,
            [](GLFWwindow *_, int32 button, int32 action, int32 mods)
            {
                switch (action)
                {
                case GLFW_PRESS:
                    event_bus::raise_event<mouse_button_press>(button);
                    break;
                case GLFW_RELEASE:
                    event_bus::raise_event<mouse_button_release>(button);
                    break;
                }
            }
        );

        glfwSetScrollCallback(window,
            [](GLFWwindow *_, double x_offset, double y_offset)
            {
                event_bus::raise_event<mouse_scroll>((float)x_offset, (float)y_offset);
            }
        );

        glfwSetCursorPosCallback(window,
            [](GLFWwindow *_, double x, double y)
            {
                event_bus::raise_event<mouse_move>((float)x, (float)y);
            }
        );

    #pragma endregion
    }

    void XPlatWindow::Destroy()
    {
        glfwDestroyWindow(window);
    }

    uint32 XPlatWindow::Width()
    {
        return _props.width;
    }

    uint32 XPlatWindow::Height()
    {
        return _props.height;
    }

    bool XPlatWindow::Vsync()
    {
        return _props.vsync;
    }

    void XPlatWindow::Vsync(bool vsync)
    {
        glfwSwapInterval(vsync);
        _props.vsync = vsync;
    }

    void XPlatWindow::Update()
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    bool XPlatWindow::IsOpen()
    {
        return !glfwWindowShouldClose(window);
    }
} // namespace polos

#endif