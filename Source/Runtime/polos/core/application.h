#pragma once

#include <memory>

#include "polos/core/window.h"
#include "polos/utils/alias.h"
#include "polos/events/window/window_close.h"
#include "polos/core/scene/scene_data.h"

namespace polos
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
    private:
        void on_window_close(window_close& p_Event);
    private:
        bool m_IsRunning;
    };

    Application* CreateApplication(void* p_PlacementPtr = nullptr);
} // namespace polos
