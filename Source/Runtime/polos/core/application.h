#pragma once

#ifndef POLOS_CORE_APPLICATION_H_
#define POLOS_CORE_APPLICATION_H_

#include <memory>

#include "polos/core/window.h"
#include "polos/utils/alias.h"
#include "polos/events/window/window_close.h"

namespace polos
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
    protected:
        Window& get_main_window();
    private:
        void on_window_close(window_close& e);
    private:
        bool               m_IsRunning;
        SharedPtr<Window> m_WindowInstance;
    };

    Application* CreateApplication(void* ptr);
} // namespace polos

#endif /* POLOS_CORE_APPLICATION_H_ */

