///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/platform_manager.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/communication/window_close.hpp"
#include "polos/communication/window_focus.hpp"
#include "polos/communication/window_framebuffer_resize.hpp"
#include "polos/logging/log_macros.hpp"

#include <sys/inotify.h>

#include <GLFW/glfw3.h>

namespace polos::platform
{

namespace
{

#ifndef NDEBUG

enum class GlfwLogLevel : std::uint8_t
{
    Warn,
    Error,
    Critical
};

struct GlfwErrorBehavior
{
    GlfwLogLevel level;
    char const*  message;
};

GlfwErrorBehavior GetGlfwErrorBehavior(std::int32_t t_error_code)
{
    switch (t_error_code)
    {
        case GLFW_INVALID_ENUM:
            return {
                .level   = GlfwLogLevel::Warn,
                .message = "GLFW received an invalid enum to it's function! Desc: {0}"};
        case GLFW_INVALID_VALUE:
            return {
                .level   = GlfwLogLevel::Warn,
                .message = "GLFW received an invalid value to it's function! Desc: {0}"};
        case GLFW_OUT_OF_MEMORY:
            return {
                .level   = GlfwLogLevel::Critical,
                .message = "A memory allocation failed within GLFW or the operating system! Desc: {0}"};
        case GLFW_API_UNAVAILABLE:
            return {
                .level   = GlfwLogLevel::Error,
                .message = "GLFW could not find support for the requested API on the system! Desc: {0}"};
        case GLFW_FORMAT_UNAVAILABLE:
            return {.level = GlfwLogLevel::Error, .message = "The requested pixel format is not supported! Desc: {0}"};
        default: return {.level = GlfwLogLevel::Error, .message = ""};
    }
}


void GlfwErrorCallback(std::int32_t t_error_code, const char* t_description)
{
    auto const behavior = GetGlfwErrorBehavior(t_error_code);
    if (behavior.message == nullptr)
    {
        LogError("Unknown GLFW error code");
        return;
    }

    switch (behavior.level)
    {
        case GlfwLogLevel::Warn: LogWarn("{} {}", behavior.message, t_description); break;
        case GlfwLogLevel::Error: LogError("{} {}", behavior.message, t_description); break;
        case GlfwLogLevel::Critical: LogCritical("{} {}", behavior.message, t_description); break;
    }
}
#endif

void OnWindowClose()
{
    communication::DispatchDefer<communication::engine_terminate>();
}

void OnEndFrame()
{
    glfwPollEvents();
}

}// namespace

PlatformManager* PlatformManager::s_instance{nullptr};

PlatformManager::PlatformManager()
{
    using namespace polos::communication;

    Subscribe<end_frame>([](end_frame&) {
        OnEndFrame();
    });

    Subscribe<window_close>([](window_close&) {
        OnWindowClose();
    });

    Subscribe<engine_terminate>([this](engine_terminate&) {
        on_engine_terminate();
    });

#if defined(__linux__)
    std::string const session_type(std::getenv("XDG_SESSION_TYPE"));// NOLINT

    if (session_type == "wayland")
    {
        LogInfo("GLFW: Setting platform to Wayland");
        glfwInitHint(GLFW_PLATFORM_WAYLAND, GLFW_TRUE);
    }
    if (session_type == "x11")
    {
        LogInfo("GLFW: Setting platform to X11");
        glfwInitHint(GLFW_PLATFORM_X11, GLFW_TRUE);
    }
#endif

    glfwInitHint(GLFW_PLATFORM_WAYLAND, GLFW_TRUE);

    if (!glfwInit())// NOLINT
    {
        LogCritical("Could not initialize GLFW!");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#ifndef NDEBUG
    glfwSetErrorCallback(GlfwErrorCallback);
#endif// !NDEBUG
}

PlatformManager& PlatformManager::Instance()
{
    return *s_instance;
}

bool PlatformManager::CreateNewWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title)
{
    std::string const title(t_title);
    m_window = glfwCreateWindow(t_width, t_height, title.c_str(), nullptr, nullptr);
    if (nullptr == m_window)
    {
        LogCritical("Could not create window!");
        return false;
    }

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* t_handle) {
        communication::DispatchDefer<communication::window_close>(t_handle);
    });

    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* /**/, std::int32_t t_is_focused) {
        communication::DispatchDefer<communication::window_focus>(t_is_focused);
    });

    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* /**/, std::int32_t t_new_width, std::int32_t t_new_height) {
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
    std::string const title(t_title);
    glfwSetWindowTitle(m_window, title.c_str());
}

GLFWwindow* PlatformManager::GetMainWindow() const
{
    return m_window;
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
