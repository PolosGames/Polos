#include "polos/polos_pch.h"

#if defined(USE_OPENGL) || defined(USE_VULKAN)

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "polos/core/window_system.h"
#include "polos/core/window.h"
#include "polos/core/log.h"
#include "polos/core/event_bus.h"
#include "polos/platform/xplat/x_plat_window_callbacks.h"
#include "polos/utils/guid.h"

namespace polos
{
    WindowSystem* WindowSystem::s_Instance;

    void WindowSystem::Startup()
    {
        s_Instance = this;
        if (!m_IsInitialized)
        {
            glfwSetErrorCallback(GLFWErrorCallback);
            int r = glfwInit(); static_cast<void>(r);
            ASSERTSTR(r == GLFW_TRUE, "Failed to initialize GLFW!");
            m_IsInitialized = true;
        }
        
        SUB_TO_EVENT_MEM_FUN(window_close, on_window_close);

        m_GfxContext = std::make_shared<GraphicsContext>();
    }
    
    void WindowSystem::Shutdown()
    {
        glfwTerminate();
        s_Instance = nullptr;
    }

    polos::GUID WindowSystem::NewWindow(window_props& new_props)
    {
        s_Instance->m_WinProps.push_back(std::move(new_props));
        auto& win_guid  = s_Instance->m_WinGUIDs.emplace_back();
        auto& win_props = s_Instance->m_WinProps.back();

        // Create the GLFW window and attach its callbacks.
        GLFWmonitor* monitor    = nullptr;
        GLFWvidmode const* mode = nullptr;
        if (win_props.fullscreen)
        {
            monitor = glfwGetPrimaryMonitor();
            mode    = glfwGetVideoMode(monitor);

            win_props.width    = mode->width;
            win_props.height   = mode->height;
            win_props.refreshRate = mode->refreshRate;
        }

        /*auto* first_win_handle = 
            s_Instance->m_WinHandles.size() >= 1 ? 
                static_cast<GLFWwindow*>(s_Instance->m_WinHandles.front()) 
                : nullptr;*/

        auto* glfw_win_ptr = glfwCreateWindow(
            win_props.width,
            win_props.height,
            win_props.title.c_str(),
            monitor,
            nullptr
        );

        s_Instance->m_WinHandles.push_back(glfw_win_ptr);

        auto* win_handle = static_cast<GLFWwindow*>(s_Instance->m_WinHandles.back());

        glfwSetWindowUserPointer(win_handle, &win_props);
        glfwMakeContextCurrent(win_handle);

        // If it's the first window, it's the main window, so create the gfx
        // context with it.
        if (s_Instance->m_WinProps.size() == 1)
        {
            s_Instance->m_GfxContext->Initialize(win_handle);
        }

        glfwSwapInterval(win_props.vsync);

        glfwSetWindowCloseCallback(win_handle, WindowCloseCallback);
        glfwSetWindowFocusCallback(win_handle, WindowFocusCallback);
        glfwSetWindowSizeCallback(win_handle, WindowSizeCallback);
        glfwSetWindowIconifyCallback(win_handle, WindowIconifyCallback);
        glfwSetWindowMaximizeCallback(win_handle, WindowMaximizeCallback);
        glfwSetFramebufferSizeCallback(win_handle, FramebufferSizeCallback);

        glfwSetKeyCallback(win_handle, KeyCallback);
        glfwSetCharCallback(win_handle, CharCallback);

        glfwSetMouseButtonCallback(win_handle, MouseButtonCallback);
        glfwSetScrollCallback(win_handle, ScrollCallback);
        glfwSetCursorPosCallback(win_handle, CursorPosCallback);

        return win_guid;
    }

    void WindowSystem::SwitchWindow(polos::GUID guid)
    {
        auto* handle = static_cast<GLFWwindow*>(GetWindowHandle(guid));
        glfwMakeContextCurrent(handle);
    }

    polos::GUID WindowSystem::GetAppWindowGUID()
    {
        if (s_Instance->m_WinGUIDs.size() < 1)
        {
            LOG_ENGINE_ERROR("There is no main app window! Returning nullopt.");
            return GUID{};
        }

        return s_Instance->m_WinGUIDs.front();
    }

    Optional<window_props> WindowSystem::GetAppWindowProps()
    {
        if (s_Instance->m_WinGUIDs.size() < 1)
        {
            LOG_ENGINE_ERROR("There is no main app window! Returning nullopt.");
            return std::nullopt;
        }

        return s_Instance->m_WinProps.front();
    }

    void* WindowSystem::GetAppWindowHandle()
    {
        if (s_Instance->m_WinGUIDs.size() < 1)
        {
            LOG_ENGINE_ERROR("There is no main app window! Returning NULL.");
            return nullptr;
        }

        return s_Instance->m_WinHandles.front();
    }

    Optional<window_props> WindowSystem::GetWindowProps(polos::GUID window_guid)
    {
        auto i = find_index_with_guid(window_guid);

        if (i == s_Instance->m_WinProps.size())
        {
            LOG_ENGINE_WARN("A window with the given guid could not be found. Returning nullopt.");
            return std::nullopt;
        };

        return s_Instance->m_WinProps[i];
    }

    void* WindowSystem::GetWindowHandle(polos::GUID window_guid)
    {
        auto i = find_index_with_guid(window_guid);

        if (i == s_Instance->m_WinHandles.size())
        {
            LOG_ENGINE_WARN("A window with the given guid could not be found. Returning null.");
            return nullptr;
        };

        return s_Instance->m_WinHandles[i];
    }

    bool WindowSystem::IsInitialized()
    {
        return s_Instance->m_IsInitialized;
    }

    void WindowSystem::Update()
    {
        auto& windows = s_Instance->m_WinHandles;
        for (auto const& handle : windows)
        {
            glfwMakeContextCurrent(static_cast<GLFWwindow*>(handle));
            glfwPollEvents();
            glfwSwapBuffers(static_cast<GLFWwindow*>(handle));
        }
    }

    void WindowSystem::on_window_close(window_close& e)
    {
        int i = 0;
        for (; i < m_WinHandles.size(); i++)
        {
            if (m_WinHandles[i] == e.winHandle)
            {
                break;
            }
        }

        // Delete that window from the window list.
        m_WinHandles.erase(m_WinHandles.begin() + i);
        m_WinProps.erase(m_WinProps.begin() + i);

        // Finally, destroy it with glfw function.
        glfwDestroyWindow(static_cast<GLFWwindow*>(e.winHandle));
    }

    std::size_t WindowSystem::find_index_with_guid(polos::GUID window_guid)
    {
        auto& guid_list = s_Instance->m_WinGUIDs;

        std::size_t i = 0;
        for (; i < guid_list.size(); i++)
        {
            if (guid_list[i] == window_guid)
            {
                break;
            }
        }

        return i;
    }

} // namespace polos

#endif