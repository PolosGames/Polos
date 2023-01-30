#include "polos/polos_pch.h"

#include "polos/core/log.h"
#include "polos/time/timer.h"
#include "polos/memory/linear_allocator.h"
#include "polos/core/event_bus.h"
#include "polos/renderer/renderer.h"
#include "polos/core/application.h"
#include "polos/context/shader_lib.h"
#include "polos/core/update_queue.h"
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
        time::Timer::OnStartUp();

        // Allocate enough memory for the whole engine.
        size_t l_needed_memory = 512
            + sizeof(Log)
            + sizeof(EventBus)
            + sizeof(Renderer)
            + sizeof(ShaderLib)
            + sizeof(UpdateQueue)
            + sizeof(WindowSystem)
            + sizeof(SoundEngine)
        ;
            
        memory::LinearAllocator l_EngineMemory(l_needed_memory);

        // Allocate memory for the systems
        auto* l_Log       = l_EngineMemory.New<Log>();
        auto* l_EventBus = l_EngineMemory.New<EventBus>();
        auto* l_Renderer  = l_EngineMemory.New<Renderer>();
        auto* l_WindowSystem = l_EngineMemory.New<WindowSystem>();
        auto* l_SoundEngine = l_EngineMemory.New<SoundEngine>();
        auto* l_ComponentMemory = l_EngineMemory.New<ecs::ComponentMemory>();

        // Single instance subsystems
        auto* l_ShaderLib = l_EngineMemory.New<ShaderLib>();
        auto* l_UpdateQueue  = l_EngineMemory.New<UpdateQueue>();

        // Startup for systems
        l_Log            ->Startup();
        l_EventBus       ->Startup();
        l_WindowSystem   ->Startup();
        l_Renderer       ->Startup();
        l_ComponentMemory->Startup();

        ShaderLib::s_Instance       = l_ShaderLib;
        UpdateQueue::s_Instance     = l_UpdateQueue;
        l_SoundEngine->Startup();

        // This ensures that all events are fired (instantiated) once,
        // so that every one of their id's get created.
        RaiseAllEvents();

        Application* l_App = CreateApplication();
        l_App->Run();
        delete l_App;

        // Shutdown sequence
        l_ComponentMemory->Shutdown();
        l_SoundEngine    ->Shutdown();
        l_Renderer       ->Shutdown();
        l_WindowSystem   ->Shutdown();
        l_EventBus       ->Shutdown();
        l_Log            ->Shutdown();

        l_EngineMemory.Delete(l_ComponentMemory);
        l_EngineMemory.Delete(l_SoundEngine);
        l_EngineMemory.Delete(l_UpdateQueue);
        l_EngineMemory.Delete(l_ShaderLib);
        l_EngineMemory.Delete(l_WindowSystem);
        l_EngineMemory.Delete(l_Renderer);
        l_EngineMemory.Delete(l_EventBus);
        l_EngineMemory.Delete(l_Log);
    }
} // namespace polos
