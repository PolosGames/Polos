#include "polos/core/log.h"
#include "polos/core/time/timer.h"
#include "polos/core/memory/linear_allocator.h"
#include "polos/core/event_bus.h"
#include "polos/core/events/events.h"
#include "polos/core/window_system.h"
#include "polos/core/sound/sound_engine.h"
#include "polos/core/ecs/component_memory.h"
#include "polos/core/application.h"
#include "polos/core/ecs/components/components.h"
#include "polos/core/resource/resource_cache.h"
#include "polos/core/resource/resource.h"
#include "polos/graphics/renderer.h"
#include "polos/graphics/shader_lib.h"

#include "engine.h"

namespace polos
{
    static void RaiseAllEvents()
    {
        // Invokes all the events by once, so we don't get the same id's.
        // This literally smells bad, but will do for 
        engine_stop{};
        engine_update{};
        engine_late_update{};
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

    static void InitializeComponentIds()
    {
        ecs::info_component{};
        ecs::transform_component{};
        ecs::texture2d_component{};
        ecs::material_component{};
        ecs::camera_component{};
        ecs::animator_component{};
    }

    Engine* Engine::s_Instance;

    void Engine::Run()
    {
        time::Timer::Startup();

        // Allocate enough memory for the whole engine.
        size_t needed_memory = 512
            + sizeof(Engine)
            + sizeof(Log)
            + sizeof(EventBus)
            + sizeof(Renderer)
            + sizeof(SoundEngine)
            + sizeof(ecs::ComponentMemory)
            + sizeof(ShaderLib)
            + sizeof(resource::ResourceCache<resource::image>)
        ;
            
        LinearAllocator engine_memory(needed_memory);

        InitializeComponentIds();

        // This ensures that all events are fired (instantiated) once,
        // so that every one of their id's get created.
        RaiseAllEvents();

        auto* engine = engine_memory.New<Engine>();
        s_Instance   = engine;

        // Allocate memory for the systems
        auto* log              = engine_memory.New<Log>();
        auto* event_bus        = engine_memory.New<EventBus>();
        auto* renderer         = engine_memory.New<Renderer>();
        auto* window_system    = engine_memory.New<WindowSystem>();
        auto* sound_engine     = engine_memory.New<SoundEngine>();
        auto* component_memory = engine_memory.New<ecs::ComponentMemory>();
        auto* shader_lib       = engine_memory.New<ShaderLib>();
        auto* image_cache      = engine_memory.New<resource::ResourceCache<resource::image>>();

        // Startup for systems
        for (auto& cb : s_Instance->m_StartupSequence)
        {
            std::invoke(cb);
        }

        Application* l_App = CreateApplication();
        l_App->Run();
        delete l_App;

        // Shutdown sequence
        std::ranges::reverse_view rv{s_Instance->m_ShutdownSequnce};
        for (auto& cb : rv)
        {
            std::invoke(cb);
        }

        engine_memory.Delete(image_cache);
        engine_memory.Delete(shader_lib);
        engine_memory.Delete(component_memory);
        engine_memory.Delete(sound_engine);
        engine_memory.Delete(window_system);
        engine_memory.Delete(renderer);
        engine_memory.Delete(event_bus);
        engine_memory.Delete(log);
    }
    
    void Engine::SubscribeToStartup(Delegate<void()> p_Delegate)
    {
        s_Instance->m_StartupSequence.push_back(std::move(p_Delegate));
    }

    void Engine::SubscribeToShutdown(Delegate<void()> p_Delegate)
    {
        s_Instance->m_ShutdownSequnce.push_back(std::move(p_Delegate));
    }
} // namespace polos
