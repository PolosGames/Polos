//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/platform/window_manager.hpp"

#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/window_events.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/gl_render_context.hpp"

#include <glad/glad.h>

namespace polos::platform
{

namespace
{

#ifndef NDEBUG
void GLFWErrorCallback(int p_ErrorCode, const char* p_Description)
{
    switch (p_ErrorCode)
    {
        case GLFW_INVALID_ENUM:
            LogWarn("GLFW received an invalid enum to it's function! Desc: {0}", p_Description);
            break;
        case GLFW_INVALID_VALUE:
            LogWarn("GLFW received an invalid value to it's function! Desc: {0}", p_Description);
            break;
        case GLFW_OUT_OF_MEMORY:
            LogCritical("A memory allocation failed within GLFW or the operating system! Desc: {0}", p_Description);
            break;
        case GLFW_API_UNAVAILABLE:
            LogError("GLFW could not find support for the requested API on the system! Desc: {0}", p_Description);
            break;
        case GLFW_FORMAT_UNAVAILABLE:
            LogError("The requested pixel format is not supported! Desc: {0}", p_Description);
            break;
    }
}
#endif

void WindowCloseCallback(GLFWwindow* t_handle)
{
    // Raise the event with the window handle so we know which window to close.
    communication::Dispatch<communication::window_close>(t_handle);
}

void WindowFocusCallback(GLFWwindow* p_Window, std::int32_t t_is_focused)
{
    if (p_Window != glfwGetCurrentContext())
    {
        glfwMakeContextCurrent(p_Window);
    }
    communication::Dispatch<communication::window_focus>(t_is_focused);
}

void FramebufferSizeCallback(GLFWwindow* /*p_Window*/, std::int32_t t_width, std::int32_t t_height)
{
    communication::Dispatch<communication::window_framebuffer_resize>(t_width, t_height);
    glViewport(0, 0, t_width, t_height);
}

}// namespace

class WindowManager::Impl
{
public:
    Impl()
    {
        communication::Subscribe<communication::engine_update>(
            [this](communication::engine_update& t_event)
            {
                OnUpdate(t_event);
            });
        communication::Subscribe<communication::window_close>(
            [this](communication::window_close& t_event)
            {
                OnWindowClose(t_event);
            });

        if (!glfwInit())
        {
            LogCritical("Could not initialize GLFW!");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
        glfwSetErrorCallback(GLFWErrorCallback);
#endif// !NDEBUG

        m_window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
        if (nullptr == m_window)
        {
            LogCritical("Could not create main window!");
            return;
        }

        glfwMakeContextCurrent(m_window);

        if (!rendering::InitializeRenderContext())
        {
            LogCritical("Aborting initialization of WindowManager since GL was not initialized!");
            return;
        }

        glfwSwapInterval(1);

        glfwSetWindowCloseCallback(m_window, WindowCloseCallback);
        glfwSetWindowFocusCallback(m_window, WindowFocusCallback);
        glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
    }

    ~Impl()
    {
    }

    void OnUpdate(communication::engine_update&)
    {
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    void OnWindowClose(communication::window_close&)
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

private:
    GLFWwindow* m_window{nullptr};
};

WindowManager::WindowManager()
{
    m_impl = new Impl();
}

WindowManager::~WindowManager()
{
    delete m_impl;
}

WindowManager& WindowManager::Instance()
{
    static WindowManager win_mgr;
    return win_mgr;
}

}// namespace polos::platform
