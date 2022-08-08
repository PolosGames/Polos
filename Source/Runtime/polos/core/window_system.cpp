#include "polos/polos_pch.h"

#include "polos/core/window_system.h"
#include "polos/core/log.h"

namespace polos
{
    WindowSystem* WindowSystem::m_Instance;

    void WindowSystem::Startup()
    {
        m_Instance = this;
    }

    void WindowSystem::Shutdown()
    {
        m_Instance = nullptr;
    }

    SharedPtr<IWindow> WindowSystem::GetMainWindow()
    {
        if (m_Instance->m_Windows.size() != 0)
        {
            return m_Instance->m_Windows[0];
        }

        LOG_ENGINE_WARN("Trying to reach the main window, but it doesn't exist.");
        return nullptr;
    }
} // namespace polos