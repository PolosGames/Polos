///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/platform_manager.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/communication/rendering_module_reload.hpp"
#include "polos/communication/window_close.hpp"
#include "polos/communication/window_focus.hpp"
#include "polos/communication/window_framebuffer_resize.hpp"
#include "polos/logging/log_macros.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <sys/inotify.h>

namespace polos::rendering
{
struct VulkanState;
}

namespace polos::platform
{

PlatformManager* PlatformManager::s_instance{nullptr};

PlatformManager::PlatformManager()
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

PlatformManager::~PlatformManager() {}

PlatformManager& PlatformManager::Instance()
{
    return *s_instance;
}

bool PlatformManager::CreateNewWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title)
{
    m_window = glfwCreateWindow(t_width, t_height, t_title.data(), NULL, NULL);
    if (nullptr == m_window)
    {
        LogCritical("Could not create window!");
        return false;
    }

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* t_handle) {
        communication::DispatchDefer<communication::window_close>(t_handle);
    });

    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* p_Window, std::int32_t t_is_focused) {
        if (p_Window != glfwGetCurrentContext())
        {
            glfwMakeContextCurrent(p_Window);
        }
        communication::DispatchDefer<communication::window_focus>(t_is_focused);
    });

    glfwSetFramebufferSizeCallback(
        m_window,
        [](GLFWwindow* /*p_Window*/, std::int32_t t_new_width, std::int32_t t_new_height) {
            communication::DispatchDefer<communication::window_framebuffer_resize>(t_new_width, t_new_height);
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
                polos::communication::DispatchDefer<communication::key_release>(t_key);
            }
        });

    return true;
}

void PlatformManager::ChangeWindowTitle(std::string_view const t_title)
{
    glfwSetWindowTitle(m_window, t_title.data());
}

GLFWwindow* PlatformManager::GetMainWindow() const
{
    return m_window;
}

void PlatformManager::on_end_frame() const
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void PlatformManager::on_window_close()
{
    communication::DispatchDefer<communication::engine_terminate>();
}

void PlatformManager::on_engine_terminate()
{
    if (nullptr == m_window)
    {
        return;
    }

    m_window = nullptr;
    LogInfo("Terminating PlatformManager...");

    glfwTerminate();
}

}// namespace polos::platform
