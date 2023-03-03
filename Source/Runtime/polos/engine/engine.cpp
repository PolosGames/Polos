#include "polos/polos_pch.h"

#include "engine.h"

#include "polos/core/log.h"
#include "polos/time/timer.h"
#include "polos/memory/linear_allocator.h"
#include "polos/core/event_bus.h"
#include "polos/renderer/renderer.h"
#include "polos/core/application.h"
#include "polos/context/shader_lib.h"
#include "polos/core/update_queue.h"
#include "polos/utils/stringid.h"
#include "polos/events/events.h"
#include "polos/core/window_system.h"

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
        time::Timer::Startup();

        // Allocate enough memory for the whole engine.
        size_t needed_memory = 512
            + sizeof(Log)
            + sizeof(EventBus)
            + sizeof(Renderer)
            + sizeof(ShaderLib)
            + sizeof(UpdateQueue)
        ;
            
        memory::LinearAllocator engine_memory(needed_memory);

        // Allocate memory for the systems
        auto* log           = engine_memory.New<Log>();
        auto* event_bus     = engine_memory.New<EventBus>();
        auto* renderer      = engine_memory.New<Renderer>();
        auto* window_system = engine_memory.New<WindowSystem>();

        // Single instance subsystems
        auto* shader_lib = engine_memory.New<ShaderLib>();
        auto* update_queue  = engine_memory.New<UpdateQueue>();

        // Startup for systems
        log          ->Startup();
        event_bus    ->Startup();
        renderer     ->Startup();
        window_system->Startup();

        ShaderLib::s_Instance       = shader_lib;
        UpdateQueue::s_Instance     = update_queue;

        // This ensures that all events are fired (instantiated) once,
        // so that every one of their id's get created.
        RaiseAllEvents();

        Application* p_app = CreateApplication();
        p_app->Run();
        delete p_app;

        // Shutdown sequence
        window_system->Shutdown();
        renderer     ->Shutdown();
        event_bus    ->Shutdown();
        log          ->Shutdown();
        
        engine_memory.Delete(update_queue);
        engine_memory.Delete(shader_lib);
        engine_memory.Delete(renderer);
        engine_memory.Delete(event_bus);
        engine_memory.Delete(log);
    }
} // namespace polos
