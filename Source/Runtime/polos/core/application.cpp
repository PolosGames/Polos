#include "application.h"

#include <glad/glad.h>

#include "polos/core/event_bus.h"
#include "polos/core/events/events.h"
#include "polos/core/time/timer.h"
#include "polos/gui/gui.h"
#include "polos/core/window_system.h"
#include "polos/core/resource/resource_cache.h"
#include "polos/core/resource/resource.h"

namespace polos
{
    Application::Application()
        : m_IsRunning{true}
    {
        SUB_TO_EVENT_MEM_FUN(window_close, on_window_close);
    
        resource::ResourceCache<resource::image>::NewLoader<resource::ImageLoaderStb>();
    }


    Application::~Application()
    {
        UNSUB_FROM_EVENT_MEM_FUN(window_close, on_window_close);
    }

    void Application::Run()
    {
        float delta_time{};
        int64 end   = time::Timer::Now();
        int64 start = time::Timer::Now();
        
        glClearColor(0.45f, 0.55f, 0.6f, 1.0f);

        Gui::Setup();
        
        while (m_IsRunning)
        {
            end        = time::Timer::Now();
            delta_time = static_cast<float>(end - start) * 0.001f * 0.001f;

            //LOG_ENGINE_INFO("FPS: {}", 1 / delta_time);

            start      = time::Timer::Now();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            Gui::Begin();

            EventBus::RaiseEvent<engine_update>();
            
            Gui::End();

            EventBus::RaiseEvent<engine_late_update>();
        }
        
        Gui::Shutdown();
    }
    
    void Application::on_window_close(window_close&)
    {
        m_IsRunning = false;
    }
} // namespace polos