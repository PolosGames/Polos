
#if defined(USE_OPENGL) || defined(USE_VULKAN)

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "polos/core/window_system.h"
#include "polos/core/window.h"
#include "polos/core/log.h"
#include "polos/core/event_bus.h"
#include "polos/utils/guid.h"
#include "polos/graphics/renderer.h"

#include "polos/core/events/input/input_events.h"
#include "polos/core/events/window/window_events.h"

namespace polos
{
    namespace
    {
        void GLFWErrorCallback(int p_ErrorCode, const char* p_Description)
        {
            switch (p_ErrorCode)
            {
                case GLFW_INVALID_ENUM:
                    LOG_ENGINE_WARN("GLFW received an invalid enum to it's function! Desc: {0}", p_Description);
                    break;
                case GLFW_INVALID_VALUE:
                    LOG_ENGINE_WARN("GLFW received an invalid value to it's function! Desc: {0}", p_Description);
                    break;
                case GLFW_OUT_OF_MEMORY:
                    LOG_ENGINE_CRITICAL("A memory allocation failed within GLFW or the operating system! Desc: {0}", p_Description);
                    break;
                case GLFW_API_UNAVAILABLE:
                    LOG_ENGINE_ERROR("GLFW could not find support for the requested API on the system! Desc: {0}", p_Description);
                    break;
                case GLFW_FORMAT_UNAVAILABLE:
                    LOG_ENGINE_ERROR("The requested pixel format is not supported! Desc: {0}", p_Description);
                    break;
            }
        }

        void WindowCloseCallback(GLFWwindow* p_Window)
        {
            // Raise the event with the window handle so we know which window to
            // close.
            EventBus::RaiseEvent<window_close>(p_Window);
        }

        void WindowFocusCallback(GLFWwindow* p_Window, int32 p_Focused)
        {
            if (p_Window != glfwGetCurrentContext())
            {
                glfwMakeContextCurrent(p_Window);
            }
            EventBus::RaiseEvent<window_focus>(p_Focused);
        }

        void WindowSizeCallback(GLFWwindow* p_Window, int32 p_Width, int32 p_Height)
        {
            window_props* props = static_cast<window_props*>(glfwGetWindowUserPointer(p_Window));
            props->width        = p_Width;
            props->height       = p_Height;
            EventBus::RaiseEvent<window_resize>(p_Width, p_Height);
        }

        void WindowIconifyCallback(GLFWwindow* /*p_Window*/, int32 p_Iconified)
        {
            EventBus::RaiseEvent<window_iconify>(p_Iconified);
        }

        void WindowMaximizeCallback(GLFWwindow* /*p_Window*/, int32 p_Maximized)
        {
            EventBus::RaiseEvent<window_maximize>(p_Maximized);
        }

        void FramebufferSizeCallback(GLFWwindow* /*p_Window*/, int32 p_Width, int32 p_Height)
        {
            EventBus::RaiseEvent<window_framebuffer_size>();
            glViewport(0, 0, p_Width, p_Height);
        }

        void KeyCallback(
            GLFWwindow* /*p_Window*/,
            int32 p_Key,
            int32 /*p_Scancode*/,
            int32 p_Action,
            int32 /*p_Mods*/
        )
        {
            switch (p_Action)
            {
                case GLFW_PRESS:
                    EventBus::RaiseEvent<key_press>(p_Key);
                    break;
                case GLFW_RELEASE:
                    EventBus::RaiseEvent<key_release>(p_Key);
                    break;
            }
        }

        void CharCallback(GLFWwindow* /*p_Window*/, uint32 p_Unicode)
        {
            EventBus::RaiseEvent<char_type>(p_Unicode);
        }

        void MouseButtonCallback(
            GLFWwindow* /*p_Window*/,
            int32 p_Button,
            int32 p_Action,
            int32 /*p_Mods*/
        )
        {
            switch (p_Action)
            {
                case GLFW_PRESS:
                    EventBus::RaiseEvent<mouse_button_press>(p_Button);
                    break;
                case GLFW_RELEASE:
                    EventBus::RaiseEvent<mouse_button_release>(p_Button);
                    break;
            }
        }

        void ScrollCallback(
            GLFWwindow* /*p_Window*/,
            double p_XOffset,
            double p_YOffset)
        {
            EventBus::RaiseEvent<mouse_scroll>(static_cast<float>(p_XOffset), static_cast<float>(p_YOffset));
        }

        void CursorPosCallback(GLFWwindow* /*p_Window*/, double p_XOffset, double p_YOffset)
        {
            EventBus::RaiseEvent<mouse_move>(static_cast<float>(p_XOffset), static_cast<float>(p_YOffset));
        }
    }

    WindowSystem* WindowSystem::s_Instance;

    void WindowSystem::Startup()
    {
        s_Instance = this;
        if (!m_IsInitialized)
        {
            glfwSetErrorCallback(GLFWErrorCallback);
            int r = glfwInit(); static_cast<void>(r);
            ASSERT(r == GLFW_TRUE, "Failed to initialize GLFW!");
            m_IsInitialized = true;

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        }
        
        SUB_TO_EVENT_MEM_FUN(window_close, on_window_close);

        m_GfxContext = std::make_shared<GraphicsContext>();
    }
    
    void WindowSystem::Shutdown()
    {
        glfwTerminate();
        s_Instance = nullptr;
    }

    polos::GUID WindowSystem::NewWindow(window_props& p_NewProps)
    {
        s_Instance->m_WinProps.push_back(std::move(p_NewProps));
        auto& win_guid  = s_Instance->m_WinGUIDs.emplace_back();
        auto& win_props = s_Instance->m_WinProps.back();

        // Create the GLFW window and attach its callbacks.
        GLFWmonitor* monitor    = glfwGetPrimaryMonitor();
        GLFWvidmode const* mode = glfwGetVideoMode(monitor);

        auto* glfw_win_ptr = glfwCreateWindow(
            win_props.width,
            win_props.height,
            win_props.title.c_str(),
            nullptr,
            nullptr
        );

        s_Instance->m_WinHandles.push_back(glfw_win_ptr);

        auto* win_handle = static_cast<GLFWwindow*>(s_Instance->m_WinHandles.back());

        glfwSetWindowUserPointer(win_handle, &win_props);
        glfwMakeContextCurrent(win_handle);

        int32 pos_x, pos_y;
        glfwGetWindowPos(glfw_win_ptr, &pos_x, &pos_y);
        

        if (win_props.fullscreen)
        {
            int32 refresh_rate = win_props.vsync ? mode->refreshRate : win_props.refreshRate;

            glfwSetWindowMonitor(glfw_win_ptr, monitor, pos_x, pos_y, mode->width, mode->height, refresh_rate);
        }
        else
        {
            int32 window_width, window_height;
            glfwGetWindowSize(glfw_win_ptr, &window_width, &window_height);
            win_props.width = window_width;
            win_props.height = window_height;

            glfwMaximizeWindow(glfw_win_ptr);
        }

        // If it's the first window, it's the main window, so create the gfx
        // context with it.
        if (s_Instance->m_WinProps.size() == 1)
        {
            s_Instance->m_GfxContext->Initialize(win_handle);

            // Also set the renderer's window handle here.
            Renderer::SetMainWindowHandle(win_guid);
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

    void WindowSystem::SwitchWindow(polos::GUID p_Guid)
    {
        auto* handle = static_cast<GLFWwindow*>(GetWindowHandle(p_Guid));
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

    Optional<window_props> WindowSystem::GetWindowProps(polos::GUID p_WindowGuid)
    {
        auto i = find_index_with_guid(p_WindowGuid);

        if (i == s_Instance->m_WinProps.size())
        {
            LOG_ENGINE_WARN("A window with the given guid could not be found. Returning nullopt.");
            return std::nullopt;
        }

        return s_Instance->m_WinProps[i];
    }

    void* WindowSystem::GetWindowHandle(polos::GUID p_WindowGuid)
    {
        auto i = find_index_with_guid(p_WindowGuid);

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

    void WindowSystem::on_window_close(window_close& p_Event)
    {
        int i = 0;
        for (; i < m_WinHandles.size(); i++)
        {
            if (m_WinHandles[i] == p_Event.winHandle)
            {
                break;
            }
        }

        // Delete that window from the window list.
        m_WinHandles.erase(m_WinHandles.begin() + i);
        m_WinProps.erase(m_WinProps.begin() + i);

        // Finally, destroy it with glfw function.
        glfwDestroyWindow(static_cast<GLFWwindow*>(p_Event.winHandle));
    }

    std::size_t WindowSystem::find_index_with_guid(polos::GUID p_WindowGuid)
    {
        auto& guid_list = s_Instance->m_WinGUIDs;

        std::size_t i{};
        for (; i < guid_list.size(); ++i)
        {
            if (guid_list[i] == p_WindowGuid)
            {
                break;
            }
        }

        return i;
    }

} // namespace polos

#endif