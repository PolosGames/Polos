#include "polos_pch.h"

#include "time/timer.h"
#include "memory/linear_allocator.h"
#include "log.h"
#include "event_bus.h"
#include "renderer/renderer.h"
#include "application.h"
#include "events/engine/engine_stop.h"

#include "engine.h"

namespace polos
{
    void Engine::Run()
    {
        time::Timer::OnStartUp();

        // Allocate enough memory for the whole engine.
        size_t p_needed_memory = 512
            + sizeof(Log)
            + sizeof(EventBus)
            + sizeof(Renderer)
        ;
            
        memory::LinearAllocator p_engine_memory(p_needed_memory);

        /// Allocate memory for the subsystems
        Log*      p_log       = p_engine_memory.New<Log>();
        EventBus* p_event_bus = p_engine_memory.New<EventBus>();
        Renderer* p_renderer  = p_engine_memory.New<Renderer>();

        /// Startup for subsystems
        p_log      ->Startup();
        p_event_bus->Startup();
        p_renderer ->Startup();

        Application* p_app = CreateApplication(nullptr);
        p_app->Run();

        /// Shutdown sequence
        p_renderer ->Shutdown();
        p_event_bus->Shutdown();
        p_log      ->Shutdown();
        
        p_engine_memory.Delete(p_renderer);
        p_engine_memory.Delete(p_event_bus);
        p_engine_memory.Delete(p_log);
        
        delete p_app;
    }
} // namespace polos
