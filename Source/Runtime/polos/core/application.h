#pragma once

#ifndef POLOS_CORE_APPLICATION_H_
#define POLOS_CORE_APPLICATION_H_

#include <memory>

#include "polos/core/i_window.h"
#include "polos/utils/alias.h"
#include "polos/events/window/window_close.h"

namespace polos
{
    class Application
    {
    public:
        Application(window_props&& props = window_props{});
        virtual ~Application();

        void Run();
    protected:
        IWindow& get_main_window();
    private:
        void on_window_close(window_close& e);
    private:
        std::unique_ptr<IWindow> m_WindowInstance;
        bool                     m_IsRunning;
    };

    Application* CreateApplication(void* ptr);
} // namespace polos

#endif /* POLOS_CORE_APPLICATION_H_ */

