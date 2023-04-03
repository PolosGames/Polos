#pragma once

#include "polos/core/window.h"
#include "polos/utils/alias.h"
#include "polos/core/events/window/window_close.h"
#include "polos/core/camera_controller.h"

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

        CameraController m_CameraController;
    };

    Application* CreateApplication(void* p_PlacementPtr = nullptr);
} // namespace polos
