#include "polos_pch.h"

#include <glad/glad.h>

#include "event_bus.h"
#include "events/events.h"
#include "update_queue.h"
#include "time/timer.h"
#include "gui/gui.h"

#include "application.h"

namespace polos
{
    Application::Application(window_props&& props)
        : m_WindowInstance{std::unique_ptr<IWindow>{ IWindow::NewWindow(std::forward<window_props>(props)) }},
          m_IsRunning{true}
    {
        
        EventBus::SubscribeToEvent<window_close, Application, &Application::on_window_close>(this);
    }

    void Application::Run()
    {
        
        float delta_time{};
        int64 end  { time::Timer::Now() };
        int64 start{ time::Timer::Now() };
        
        glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
        
        Gui::Setup();
        while (m_IsRunning)
        {
            end = time::Timer::Now();
            delta_time = (end - start) * 0.001f * 0.001f * 0.001f;
            start = time::Timer::Now();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            Gui::Begin();
            
            UpdateQueue::Update(delta_time);
    
            Gui::End();
            
            m_WindowInstance->Update();
        }
        
        Gui::Shutdown();
    }
    
    void Application::on_window_close(window_close& /* e */)
    {
        m_IsRunning = false;
        m_WindowInstance->Shutdown();
    }
    
    Application::~Application()
    {
    }
    
    IWindow& Application::get_main_window()
    {
        return *m_WindowInstance;
    }
} // namespace polos