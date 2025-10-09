///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/window_manager.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/communication/system_created.hpp"
#include "polos/communication/window_events.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk_instance.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace polos::rendering
{
struct VulkanState;
}
namespace polos::platform
{

namespace
{
rendering::rendering_dll_out rendering_dll;
}

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

#ifndef NDEBUG
    glfwSetErrorCallback([](std::int32_t t_error_code, const char* t_description) {
        switch (t_error_code)
        {
            case GLFW_INVALID_ENUM:
                LogWarn("GLFW received an invalid enum to it's function! Desc: {0}", t_description);
                break;
            case GLFW_INVALID_VALUE:
                LogWarn("GLFW received an invalid value to it's function! Desc: {0}", t_description);
                break;
            case GLFW_OUT_OF_MEMORY:
                LogCritical("A memory allocation failed within GLFW or the operating system! Desc: {0}", t_description);
                break;
            case GLFW_API_UNAVAILABLE:
                LogError("GLFW could not find support for the requested API on the system! Desc: {0}", t_description);
                break;
            case GLFW_FORMAT_UNAVAILABLE:
                LogError("The requested pixel format is not supported! Desc: {0}", t_description);
                break;
        }
    });
#endif// !NDEBUG
}

WindowManager::~WindowManager() {}

WindowManager& WindowManager::Instance()
{
    static WindowManager win_mgr;
    return win_mgr;
}

bool WindowManager::CreateNewWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title)
{
    m_window = glfwCreateWindow(t_width, t_height, t_title.data(), NULL, NULL);
    if (nullptr == m_window)
    {
        LogCritical("Could not create window!");
        return false;
    }

    init_vulkan();

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* t_handle) {
        communication::DispatchNow<communication::window_close>(t_handle);
    });

    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* p_Window, std::int32_t t_is_focused) {
        if (p_Window != glfwGetCurrentContext())
        {
            glfwMakeContextCurrent(p_Window);
        }
        communication::DispatchNow<communication::window_focus>(t_is_focused);
    });

    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* /*p_Window*/, std::int32_t t_width, std::int32_t t_height) {
        communication::DispatchNow<communication::window_framebuffer_resize>(t_width, t_height);
    });

    glfwSetKeyCallback(
        m_window,
        [](GLFWwindow* /*t_window*/,
           std::int32_t t_key,
           std::int32_t /*t_scancode*/,
           std::int32_t t_action,
           std::int32_t /*t_mods*/) {
            if (t_action == GLFW_RELEASE)
            {
                polos::communication::DispatchNow<communication::key_release>(t_key);
            }
        });

    return true;
}

GLFWwindow* WindowManager::GetRawWindow() const
{
    return m_window;
}

void WindowManager::UpdateRenderingModule(rendering::rendering_dll_out& t_dll_out)
{
    rendering_dll = t_dll_out;

    if (nullptr != m_window)
    {
        init_vulkan();
    }
}

void WindowManager::init_vulkan()
{
#if defined(NDEBUG)
    auto result    = rendering::InitializeVulkan(m_window);
    m_vulkan_state = *result.value();
#else
    polos::rendering::VulkanState* result = rendering_dll.initialize_vulkan_func(m_window);
    m_vulkan_state                        = *result;
#endif// NDEBUG
}

void WindowManager::on_end_frame() const
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void WindowManager::on_window_close()
{
    communication::DispatchDefer<communication::engine_terminate>();
}

void WindowManager::on_engine_terminate()
{
    if (nullptr == m_window)
    {
        return;
    }

    m_window = nullptr;
    LogInfo("Terminating WindowManager...");

#if defined(NDEBUG)
    rendering::TerminateVulkan();
#else
    if (nullptr != rendering_dll.terminate_vulkan_func)
    {
        rendering_dll.terminate_vulkan_func();
    }
#endif// NDEBUG
    glfwTerminate();
}

}// namespace polos::platform
