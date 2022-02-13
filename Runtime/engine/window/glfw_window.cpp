#include "plpch.h"

#include "glfw_window.h"

#include "debug.h"
#include "events/window/window_events.h"
#include "events/input/input_events.h"

namespace polos
{
#if defined(USE_OPENGL) || defined(USE_VULKAN)
    Window *Window::create_window(const WindowProps &props)
    {
        return new glfw_window(props);
    }
#endif
    bool glfw_window::_is_glfw_initialized = false;
    uint32 glfw_window::_glfw_window_count = 0;

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

    void glfw_window::shutdown()
    {
        glfwTerminate();
    }

    glfw_window::glfw_window(const WindowProps & props)
        : _props(props)
    {
        initialize();
    }

    void glfw_window::initialize()
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
        _context->initialize(window);

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
                WindowProps *props = static_cast<WindowProps*>(glfwGetWindowUserPointer(window));
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

    void glfw_window::destroy()
    {
        glfwDestroyWindow(window);
    }

    uint32 glfw_window::width()
    {
        return _props.width;
    }

    uint32 glfw_window::height()
    {
        return _props.height;
    }

    bool glfw_window::vsync()
    {
        return _props.vsync;
    }

    void glfw_window::vsync(bool vsync)
    {
        glfwSwapInterval(vsync);
        _props.vsync = vsync;
    }

    void glfw_window::update()
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    bool glfw_window::is_open()
    {
        return !glfwWindowShouldClose(window);
    }
}