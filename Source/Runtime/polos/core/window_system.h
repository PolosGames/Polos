#pragma once

#ifndef POLOS_CORE_WINDOWSYSTEM_H_
#define POLOS_CORE_WINDOWSYSTEM_H_

#include "polos/core/i_window.h"
#include "polos/containers/containers.h"

namespace polos
{
    class WindowSystem
    {
    public:
        void Startup();
        void Shutdown();

        static PL_NODISCARD SharedPtr<IWindow> NewWindow();
        static PL_NODISCARD SharedPtr<IWindow> GetMainWindow();
    private:
        static WindowSystem* m_Instance;

        std::vector<SharedPtr<IWindow>> m_Windows;
    };
} // namespace polos

#endif /* POLOS_CORE_WINDOWSYSTEM_H_ */