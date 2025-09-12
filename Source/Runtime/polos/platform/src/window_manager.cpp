///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/window_manager.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/window_events.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk_instance.hpp"

namespace polos::platform
{

WindowManager::WindowManager()
{
    communication::Subscribe<communication::end_frame>([this](communication::end_frame&) {
        on_end_frame();
    });

    communication::Subscribe<communication::window_close>([this](communication::window_close&) {
        on_window_close();
    });

    communication::Subscribe<communication::engine_terminate>([this](communication::engine_terminate&) {
        on_engine_terminate();
    });

    if (!glfwInit())
    {
        LogCritical("Could not initialize GLFW!");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // #ifndef NDEBUG
    //     glfwSetErrorCallback([](std::int32_t t_error_code, const char* t_description) {
    //         switch (t_error_code)
    //         {
    //             case GLFW_INVALID_ENUM:
    //                 LogWarn("GLFW received an invalid enum to it's function! Desc: {0}", t_description);
    //                 break;
    //             case GLFW_INVALID_VALUE:
    //                 LogWarn("GLFW received an invalid value to it's function! Desc: {0}", t_description);
    //                 break;
    //             case GLFW_OUT_OF_MEMORY:
    //                 LogCritical("A memory allocation failed within GLFW or the operating system! Desc: {0}", t_description);
    //                 break;
    //             case GLFW_API_UNAVAILABLE:
    //                 LogError("GLFW could not find support for the requested API on the system! Desc: {0}", t_description);
    //                 break;
    //             case GLFW_FORMAT_UNAVAILABLE:
    //                 LogError("The requested pixel format is not supported! Desc: {0}", t_description);
    //                 break;
    //         }
    //     });
    // #endif// !NDEBUG
}

WindowManager::~WindowManager() {}

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

    auto result = rendering::InitializeVulkan(m_window);
    if (!result.has_value())
    {
        LogCritical("Cannot initialize Vulkan, VkResult: {}", static_cast<std::uint32_t>(result.error()));

        communication::Dispatch<communication::engine_terminate>();

        return false;
    }

    m_vulkan_state = result.value();

    // glfwMakeContextCurrent(m_window);

    // if (!rendering::InitializeRenderContext())
    // {
    //     LogCritical("Aborting initialization of WindowManager since GL was not initialized!");
    //     return false;
    // }
    // glfwSwapInterval(1);

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* t_handle) {
        communication::Dispatch<communication::window_close>(t_handle);
    });
    // glfwSetWindowFocusCallback(m_window, [](GLFWwindow* p_Window, std::int32_t t_is_focused) {
    //     if (p_Window != glfwGetCurrentContext())
    //     {
    //         glfwMakeContextCurrent(p_Window);
    //     }
    //     communication::Dispatch<communication::window_focus>(t_is_focused);
    // });
    // glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* /*p_Window*/, std::int32_t t_width, std::int32_t t_height) {
    //     communication::Dispatch<communication::window_framebuffer_resize>(t_width, t_height);
    //     // glViewport(0, 0, t_width, t_height);
    // });

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

void WindowManager::on_window_close()
{
    if (nullptr == m_window)
    {
        return;
    }
    LogInfo("Window close event received, terminating WindowManager...");
    m_window = nullptr;
    rendering::TerminateVulkan();
    glfwTerminate();
}

void WindowManager::on_engine_terminate()
{
    if (nullptr == m_window)
    {
        return;
    }

    m_window = nullptr;
    LogInfo("Terminating WindowManager...");

    rendering::TerminateVulkan();
    glfwTerminate();
}

}// namespace polos::platform
