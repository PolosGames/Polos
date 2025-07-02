///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/window_manager.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/window_events.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/gl_render_context.hpp"

#include <glad/glad.h>

namespace polos::platform
{

WindowManager::WindowManager()
{
    communication::Subscribe<communication::end_frame>(
        [this](communication::end_frame&)
        {
            on_end_frame();
        });

    communication::Subscribe<communication::window_close>(
        [this](communication::window_close&)
        {
            on_window_close();
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
    glfwSetErrorCallback(
        [](std::int32_t t_error_code, const char* t_description)
        {
            switch (t_error_code)
            {
                case GLFW_INVALID_ENUM:
                    LogWarn("GLFW received an invalid enum to it's function! Desc: {0}", t_description);
                    break;
                case GLFW_INVALID_VALUE:
                    LogWarn("GLFW received an invalid value to it's function! Desc: {0}", t_description);
                    break;
                case GLFW_OUT_OF_MEMORY:
                    LogCritical("A memory allocation failed within GLFW or the operating system! Desc: {0}",
                                t_description);
                    break;
                case GLFW_API_UNAVAILABLE:
                    LogError("GLFW could not find support for the requested API on the system! Desc: {0}",
                             t_description);
                    break;
                case GLFW_FORMAT_UNAVAILABLE:
                    LogError("The requested pixel format is not supported! Desc: {0}", t_description);
                    break;
            }
        });
#endif// !NDEBUG
}

WindowManager::~WindowManager() = default;

WindowManager& WindowManager::Instance()
{
    static WindowManager win_mgr;
    return win_mgr;
}

bool WindowManager::CreateWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title)
{
    m_window = glfwCreateWindow(t_width, t_height, t_title.data(), NULL, NULL);
    if (nullptr == m_window)
    {
        LogCritical("Could not create window!");
        return false;
    }

    glfwMakeContextCurrent(m_window);

    if (!rendering::InitializeRenderContext())
    {
        LogCritical("Aborting initialization of WindowManager since GL was not initialized!");
        return false;
    }

    glfwSwapInterval(1);

    glViewport(0, 0, t_width, t_height);

    glfwSetWindowCloseCallback(m_window,
                               [](GLFWwindow* t_handle)
                               {
                                   communication::Dispatch<communication::window_close>(t_handle);
                               });
    glfwSetWindowFocusCallback(m_window,
                               [](GLFWwindow* p_Window, std::int32_t t_is_focused)
                               {
                                   if (p_Window != glfwGetCurrentContext())
                                   {
                                       glfwMakeContextCurrent(p_Window);
                                   }
                                   communication::Dispatch<communication::window_focus>(t_is_focused);
                               });
    glfwSetFramebufferSizeCallback(m_window,
                                   [](GLFWwindow* /*p_Window*/, std::int32_t t_width, std::int32_t t_height)
                                   {
                                       communication::Dispatch<communication::window_framebuffer_resize>(t_width,
                                                                                                         t_height);
                                       glViewport(0, 0, t_width, t_height);
                                   });

    return true;
}

GLFWwindow* WindowManager::GetRawWindow() const
{
    return m_window;
}

void WindowManager::on_end_frame() const
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void WindowManager::on_window_close() const noexcept
{
    glfwTerminate();
}

}// namespace polos::platform
