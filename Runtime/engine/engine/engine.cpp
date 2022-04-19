#include "polos_pch.h"

#include "core/log.h"
#include "time/timer.h"
#include "memory/linear_allocator.h"
#include "core/event_bus.h"
#include "renderer/renderer.h"
#include "core/application.h"
#include "renderer/context/shader_lib.h"

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
        auto* p_log       = p_engine_memory.New<Log>();
        auto* p_event_bus = p_engine_memory.New<EventBus>();
        auto* p_renderer  = p_engine_memory.New<Renderer>();
        auto* p_shaderlib = p_engine_memory.New<ShaderLib>();

        /// Startup for subsystems
        p_log      ->Startup();
        p_event_bus->Startup();
        p_renderer ->Startup();
        ShaderLib::m_Instance = p_shaderlib;

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
