///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/platform_manager.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_press.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/communication/mouse_input.hpp"
#include "polos/communication/mouse_move.hpp"
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
                .message = "GLFW received an invalid enum to it's function! Desc: {0}"
            };
        case GLFW_INVALID_VALUE:
            return {
                .level   = GlfwLogLevel::Warn,
                .message = "GLFW received an invalid value to it's function! Desc: {0}"
            };
        case GLFW_OUT_OF_MEMORY:
            return {
                .level   = GlfwLogLevel::Critical,
                .message = "A memory allocation failed within GLFW or the operating system! Desc: {0}"
            };
        case GLFW_API_UNAVAILABLE:
            return {
                .level   = GlfwLogLevel::Error,
                .message = "GLFW could not find support for the requested API on the system! Desc: {0}"
            };
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
{ communication::DispatchDefer<communication::EngineTerminate>(); }

void OnEndFrame()
{ glfwPollEvents(); }

}// namespace

PlatformManager* PlatformManager::s_instance{nullptr};

PlatformManager::PlatformManager()
{
    using namespace polos::communication;

    Subscribe<EndFrame>([](EndFrame&) {
        OnEndFrame();
    });

    Subscribe<WindowClose>([](WindowClose&) {
        OnWindowClose();
    });

    Subscribe<EngineTerminate>([this](EngineTerminate&) {
        on_engine_terminate();
    });

#if defined(__linux__)
    std::string const session_type(std::getenv("XDG_SESSION_TYPE"));// NOLINT

    if (session_type == "wayland")
    {
        LogInfo("GLFW: Setting GLFW platform to Wayland");
        glfwInitHint(GLFW_PLATFORM_WAYLAND, GLFW_TRUE);
    }
    if (session_type == "x11")
    {
        LogInfo("GLFW: Setting GLFW platform to X11");
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
{ return *s_instance; }

bool PlatformManager::CreateNewWindow(std::int32_t t_width, std::int32_t t_height, std::string_view t_title)
{
    std::string const title(t_title);
    m_window = glfwCreateWindow(t_width, t_height, title.c_str(), nullptr, nullptr);
    if (nullptr == m_window)
    {
        LogCritical("Could not create window!");
        return false;
    }

    m_window_surface = std::make_unique<GlfwWindowSurface>(m_window);

    glfwFocusWindow(m_window);
    glfwSetCursorPos(m_window, static_cast<std::double_t>(t_width) / 2.0, static_cast<std::double_t>(t_height) / 2.0);

    {
        using namespace polos::communication;

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* t_handle) {
            DispatchDefer<WindowClose>(t_handle);
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* /**/, std::int32_t t_is_focused) {
            DispatchDefer<WindowFocus>(t_is_focused);
        });

        glfwSetFramebufferSizeCallback(m_window,
                                       [](GLFWwindow* /**/, std::int32_t t_new_width, std::int32_t t_new_height) {
                                           DispatchDefer<WindowFramebufferResize>(t_new_width, t_new_height);
                                       });

        glfwSetKeyCallback(m_window,
                           [](GLFWwindow* /*t_window*/,
                              std::int32_t t_key,
                              std::int32_t /*t_scancode*/,
                              std::int32_t t_action,
                              std::int32_t /*t_mods*/) {
                               switch (t_action)
                               {
                                   case GLFW_RELEASE: DispatchDefer<KeyRelease>(t_key); break;
                                   case GLFW_PRESS: DispatchDefer<KeyPress>(t_key);
                                   default: break;
                               }
                           });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* /*t_window*/, double t_xpos, double t_ypos) {
            DispatchDefer<MouseMove>(t_xpos, t_ypos);
        });

        glfwSetMouseButtonCallback(
            m_window,
            [](GLFWwindow* t_window, std::int32_t t_button, std::int32_t t_action, std::int32_t t_mods) {
                DispatchDefer<MouseInput>(t_button, t_action);
            });
    }

    return true;
}

void PlatformManager::ChangeWindowTitle(std::string_view const t_title)
{
    std::string const title(t_title);
    glfwSetWindowTitle(m_window, title.c_str());
}

GLFWwindow* PlatformManager::GetMainWindow() const
{ return m_window; }

auto PlatformManager::GetWindowSurface() const -> rendering::IWindowSurface&
{ return *m_window_surface; }

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
