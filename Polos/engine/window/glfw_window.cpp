#include "plpch.h"

#include "glfw_window.h"
#include "utils/macros.h"

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
        : props(props)
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
        window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, &props);
        glfwMakeContextCurrent(window);
    }

    void glfw_window::destroy()
    {
        glfwDestroyWindow(window);
    }

    uint32 glfw_window::width()
    {
        return props.width;
    }

    uint32 glfw_window::height()
    {
        return props.height;
    }

    bool glfw_window::vsync()
    {
        return props.vsync;
    }

    void glfw_window::vsync(bool vsync)
    {
        glfwSwapInterval(vsync);
        props.vsync = vsync;
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