#pragma once

#ifndef POLOS_CORE_WINDOWSYSTEM_H_
#define POLOS_CORE_WINDOWSYSTEM_H_

#include "polos/core/window.h"
#include "polos/containers/containers.h"
#include "polos/context/graphics_context.h"

namespace polos
{
    class WindowSystem
    {
    public:
        void Startup();
        void Shutdown();

        PL_NODISCARD static SharedPtr<Window> NewWindow();
        PL_NODISCARD static SharedPtr<Window> GetMainWindow();
        
        PL_NODISCARD static bool IsInitialized();
    private:
        // Defined in the selected platform's window system
        void Initialize();
    private:
        static WindowSystem* m_Instance;
        static bool          s_IsInitialized;

        std::vector<SharedPtr<Window>> m_Windows;
        SharedPtr<GraphicsContext>      m_GfxContext;
    };
} // namespace polos

#endif /* POLOS_CORE_WINDOWSYSTEM_H_ */