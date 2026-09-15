///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/platform_manager.hpp"

#include "glfw_window_surface.hpp"
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

GlfwErrorBehavior GetGlfwErrorBehavior(std::int32_t tErrorCode)
{
    switch (tErrorCode)
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


void GlfwErrorCallback(std::int32_t tErrorCode, const char* tDescription)
{
    auto const behavior = GetGlfwErrorBehavior(tErrorCode);
    if (behavior.message == nullptr)
    {
        LogError("Unknown GLFW error code");
        return;
    }

    switch (behavior.level)
    {
        case GlfwLogLevel::Warn: LogWarn("{} {}", behavior.message, tDescription); break;
        case GlfwLogLevel::Error: LogError("{} {}", behavior.message, tDescription); break;
        case GlfwLogLevel::Critical: LogCritical("{} {}", behavior.message, tDescription); break;
    }
}
#endif

void OnWindowClose()
{ communication::DispatchDefer<communication::EngineTerminate>(); }

void OnEndFrame()
{ glfwPollEvents(); }

}// namespace

PlatformManager* PlatformManager::sInstance{nullptr};

PlatformManager::~PlatformManager()
{
    mWindowSurface.reset();
    if (mGlfwInitialized)
    {
        glfwTerminate();
    }
}

PlatformManager::PlatformManager()
{
    using namespace polos::communication;

    Subscribe<EndFrame>([](EndFrame&) {
        OnEndFrame();
    });

    Subscribe<WindowClose>([](WindowClose&) {
        OnWindowClose();
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
    mGlfwInitialized = true;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#ifndef NDEBUG
    glfwSetErrorCallback(GlfwErrorCallback);
#endif// !NDEBUG
}

PlatformManager& PlatformManager::Instance()
{ return *sInstance; }

bool PlatformManager::CreateNewWindow(std::int32_t tWidth, std::int32_t tHeight, std::string_view tTitle)
{
    std::string const title(tTitle);
    mWindow = glfwCreateWindow(tWidth, tHeight, title.c_str(), nullptr, nullptr);
    if (nullptr == mWindow)
    {
        LogCritical("Could not create window!");
        return false;
    }

    mWindowSurface = std::make_unique<GlfwWindowSurface>(mWindow);

    glfwFocusWindow(mWindow);
    glfwSetCursorPos(mWindow, static_cast<std::double_t>(tWidth) / 2.0, static_cast<std::double_t>(tHeight) / 2.0);

    {
        using namespace polos::communication;

        glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* tHandle) {
            DispatchDefer<WindowClose>(tHandle);
        });

        glfwSetWindowFocusCallback(mWindow, [](GLFWwindow* /**/, std::int32_t tIsFocused) {
            DispatchDefer<WindowFocus>(tIsFocused);
        });

        glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* /**/, std::int32_t tNewWidth, std::int32_t tNewHeight) {
            DispatchDefer<WindowFramebufferResize>(tNewWidth, tNewHeight);
        });

        glfwSetKeyCallback(mWindow,
                           [](GLFWwindow* /*tWindow*/,
                              std::int32_t tKey,
                              std::int32_t /*tScancode*/,
                              std::int32_t tAction,
                              std::int32_t /*tMods*/) {
                               switch (tAction)
                               {
                                   case GLFW_RELEASE: DispatchDefer<KeyRelease>(tKey); break;
                                   case GLFW_PRESS: DispatchDefer<KeyPress>(tKey);
                                   default: break;
                               }
                           });

        glfwSetCursorPosCallback(mWindow, [](GLFWwindow* /*tWindow*/, double tXpos, double tYpos) {
            DispatchDefer<MouseMove>(tXpos, tYpos);
        });

        glfwSetMouseButtonCallback(
            mWindow,
            [](GLFWwindow* tWindow, std::int32_t tButton, std::int32_t tAction, std::int32_t tMods) {
                DispatchDefer<MouseInput>(tButton, tAction);
            });
    }

    return true;
}

void PlatformManager::ChangeWindowTitle(std::string_view const tTitle)
{
    std::string const title(tTitle);
    glfwSetWindowTitle(mWindow, title.c_str());
}

GLFWwindow* PlatformManager::GetMainWindow() const
{ return mWindow; }

auto PlatformManager::GetWindowSurface() const -> rendering::IWindowSurface&
{ return *mWindowSurface; }

}// namespace polos::platform
