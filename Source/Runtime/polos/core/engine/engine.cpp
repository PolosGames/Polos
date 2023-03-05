#include "polos/polos_pch.h"

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
#include "polos/core/sound/sound_engine.h"
#include "polos/core/ecs/component_memory.h"

#include "engine.h"

namespace polos
{
    static void RaiseAllEvents()
    {
        // Invokes all the events by once, so we don't get the same id's.
        // This literally smells bad, but will do for 
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
        auto* log              = engine_memory.New<Log>();
        auto* event_bus        = engine_memory.New<EventBus>();
        auto* renderer         = engine_memory.New<Renderer>();
        auto* window_system    = engine_memory.New<WindowSystem>();
        auto* sound_engine     = engine_memory.New<SoundEngine>();
        auto* component_memory = engine_memory.New<ecs::ComponentMemory>();

        // Single instance subsystems
        auto* shader_lib = engine_memory.New<ShaderLib>();
        auto* update_queue  = engine_memory.New<UpdateQueue>();

        UpdateQueue::s_Instance     = update_queue;
        // Startup for systems
        log             ->Startup();
        event_bus       ->Startup();
        renderer        ->Startup();
        window_system   ->Startup();
        sound_engine    ->Startup();
        component_memory->Startup();

        ShaderLib::s_Instance       = shader_lib;

        // This ensures that all events are fired (instantiated) once,
        // so that every one of their id's get created.
        RaiseAllEvents();

        Application* l_App = CreateApplication();
        l_App->Run();
        delete l_App;

        // Shutdown sequence
        component_memory->Shutdown();
        sound_engine    ->Shutdown();
        renderer        ->Shutdown();
        window_system   ->Shutdown();
        event_bus       ->Shutdown();
        log             ->Shutdown();
        
        engine_memory.Delete(component_memory);
        engine_memory.Delete(sound_engine);
        engine_memory.Delete(update_queue);
        engine_memory.Delete(shader_lib);
        engine_memory.Delete(renderer);
        engine_memory.Delete(event_bus);
        engine_memory.Delete(log);
    }
} // namespace polos
