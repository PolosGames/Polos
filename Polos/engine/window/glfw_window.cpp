#include "plpch.h"

#include "glfw_window.h"
#include "utils/macros.h"

#include "events/window/window_close.h"

namespace polos
{
#if defined(USE_OPENGL) || defined(USE_VULKAN)
    window *window::create_window(const window_props &props)
    {
        return new glfw_window(props);
    }

    void window::shutdown()
    {
        glfwTerminate();
    }
#endif

    bool glfw_window::_is_glfw_initialized = false;
    uint32 glfw_window::_glfw_window_count = 0;

    glfw_window::glfw_window(const window_props & props)
        : _props(props)
    {
        initialize();
    }

    void glfw_window::initialize()
    {
        if (!_is_glfw_initialized)
        {
            int r = glfwInit();
            ASSERT(r == GLFW_TRUE, "Failed to initialize GLFW!");
            _is_glfw_initialized = true;
        }
        window = glfwCreateWindow(_props.width, _props.height, _props.title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, &_props);
        glfwMakeContextCurrent(window);

        _context = std::make_unique<graphics_context>();
        _context->initialize(window);

        glfwSetWindowCloseCallback(window, 
            [](GLFWwindow *window)
            {
                event_bus::raise_event<window_close>();
            }
        );
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