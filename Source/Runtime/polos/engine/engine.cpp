#include "polos/polos_pch.h"

#include "polos/core/log.h"
#include "polos/time/timer.h"
#include "polos/memory/linear_allocator.h"
#include "polos/core/event_bus.h"
#include "polos/renderer/renderer.h"
#include "polos/core/application.h"
#include "polos/context/shader_lib.h"
#include "polos/core/update_queue.h"
#include "polos/core/resource_manager.h"
#include "polos/utils/stringid.h"
#include "polos/events/events.h"
#include "polos/core/window_system.h"

#include "engine.h"

namespace polos
{
    static void RaiseAllEvents()
    {
        // This literally smells bad, but will do for now
        engine_stop{};
        char_type{};
        key_press{};
        key_release{};
        mouse_button_press{};
        mouse_button_release{};
        mouse_move{};
        mouse_scroll{};
        window_close{};
        window_focus{};
        window_framebuffer_size{};
        window_iconify{};
        window_maximize{};
        window_position{};
        window_resize{};
    }

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
        auto* p_winsystem = p_engine_memory.New<WindowSystem>();

        // Single instance subsystems
        auto* p_shaderlib = p_engine_memory.New<ShaderLib>();
        auto* p_update_q  = p_engine_memory.New<UpdateQueue>();
        auto* p_rsrc_mng  = p_engine_memory.New<ResourceManager>();

        // Startup for systems
        p_log      ->Startup();
        p_event_bus->Startup();
        p_renderer ->Startup();
        p_winsystem->Startup();

        ShaderLib::m_Instance       = p_shaderlib;
        UpdateQueue::m_Instance     = p_update_q;
        ResourceManager::m_Instance = p_rsrc_mng;

        // This ensures that all events are fired (instantiated) once,
        // so that every one of their id's get created.
        RaiseAllEvents();

        Application* p_app = CreateApplication();
        p_app->Run();
        delete p_app;

        // Shutdown sequence
        p_winsystem->Shutdown();
        p_renderer ->Shutdown();
        p_event_bus->Shutdown();
        p_log      ->Shutdown();
        
        p_engine_memory.Delete(p_update_q);
        p_engine_memory.Delete(p_shaderlib);
        p_engine_memory.Delete(p_renderer);
        p_engine_memory.Delete(p_event_bus);
        p_engine_memory.Delete(p_log);
    }
} // namespace polos
