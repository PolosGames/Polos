#include "polos/polos_pch.h"

#if defined(USE_OPENGL) || defined(USE_VULKAN)

#include <GLFW/glfw3.h>

#include "polos/core/window_system.h"
#include "polos/core/window.h"
#include "polos/core/log.h"

namespace polos
{
    WindowSystem* WindowSystem::m_Instance;
    bool          WindowSystem::s_IsInitialized = false;

    SharedPtr<Window> WindowSystem::GetMainWindow()
    {
        if (m_Instance->m_Windows.size() != 0)
        {
            return m_Instance->m_Windows[0];
        }

        LOG_ENGINE_WARN("Trying to reach the main window, but it doesn't exist.");
        return nullptr;
    }

    bool WindowSystem::IsInitialized()
    {
        return s_IsInitialized;
    }

    static void error_callback(int error_code, const char* description)
    {
        switch (error_code)
        {
        case GLFW_INVALID_ENUM: LOG_ENGINE_WARN("GLFW received an invalid enum to it's function! Desc: {0}", description); break;
        case GLFW_INVALID_VALUE: LOG_ENGINE_WARN("GLFW received an invalid value to it's function! Desc: {0}", description); break;
        case GLFW_OUT_OF_MEMORY: LOG_ENGINE_CRITICAL("A memory allocation failed within GLFW or the operating system! Desc: {0}", description); break;
        case GLFW_API_UNAVAILABLE: LOG_ENGINE_ERROR("GLFW could not find support for the requested API on the system! Desc: {0}", description); break;
        case GLFW_FORMAT_UNAVAILABLE: LOG_ENGINE_ERROR("The requested pixel format is not supported! Desc: {0}", description); break;
        }
    }

    void WindowSystem::Startup()
    {
        m_Instance = this;
        if (!s_IsInitialized)
        {
            glfwSetErrorCallback(error_callback);
            int r = glfwInit(); static_cast<void>(r);
            ASSERTSTR(r == GLFW_TRUE, "Failed to initialize GLFW!");
            s_IsInitialized = true;
        }

        m_GfxContext = std::make_shared<GraphicsContext>();
    }
    
    void WindowSystem::Shutdown()
    {
        glfwTerminate();
        m_Instance = nullptr;
    }

    SharedPtr<Window> WindowSystem::NewWindow()
    {
        Window* ptr = new Window();
        if (m_Instance->m_Windows.size() == 0)
        {
            ptr->m_IsMainWindow = true;
            ptr->m_GfxContext = m_Instance->m_GfxContext;
        }
        return m_Instance->m_Windows.emplace_back(ptr);
    }

} // namespace polos

#endif