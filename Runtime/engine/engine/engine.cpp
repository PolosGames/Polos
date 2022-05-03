#include "polos_pch.h"

#include "core/log.h"
#include "time/timer.h"
#include "memory/linear_allocator.h"
#include "core/event_bus.h"
#include "renderer/renderer.h"
#include "core/application.h"
#include "renderer/context/shader_lib.h"
#include "core/update_queue.h"
#include "core/resource_manager.h"

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
            + sizeof(ShaderLib)
            + sizeof(UpdateQueue)
            + sizeof(ResourceManager)
        ;
            
        memory::LinearAllocator p_engine_memory(p_needed_memory);

        // Allocate memory for the systems
        auto* p_log       = p_engine_memory.New<Log>();
        auto* p_event_bus = p_engine_memory.New<EventBus>();
        auto* p_renderer  = p_engine_memory.New<Renderer>();

        // Single instance subsystems
        auto* p_shaderlib = p_engine_memory.New<ShaderLib>();
        auto* p_update_q  = p_engine_memory.New<UpdateQueue>();
        auto* p_rsrc_mng  = p_engine_memory.New<ResourceManager>();

        // Startup for systems
        p_log      ->Startup();
        p_event_bus->Startup();
        p_renderer ->Startup();

        ShaderLib::m_Instance       = p_shaderlib;
        UpdateQueue::m_Instance     = p_update_q;
        ResourceManager::m_Instance = p_rsrc_mng;

        Application* p_app = CreateApplication(nullptr);
        p_app->Run();


        // Shutdown sequence
        p_renderer ->Shutdown();
        p_event_bus->Shutdown();
        p_log      ->Shutdown();
        
        p_engine_memory.Delete(p_update_q);
        p_engine_memory.Delete(p_shaderlib);
        p_engine_memory.Delete(p_renderer);
        p_engine_memory.Delete(p_event_bus);
        p_engine_memory.Delete(p_log);
        
        delete p_app;
    }
} // namespace polos
