///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/main_loop.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/process_input.hpp"
#include "polos/communication/render_update.hpp"
#include "polos/communication/window_close.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/ecs/scene_manager.hpp"
#include "polos/ecs/system_registry.hpp"
#include "polos/ecs/systems/editor_camera_system.hpp"
#include "polos/ecs/systems/light_collection_system.hpp"
#include "polos/ecs/systems/mesh_instance_sync_system.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/utils/profiling.hpp"
#include "polos/utils/time.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <thread>

namespace polos::core
{

MainLoop::MainLoop()
{
    LogInfo("MainLoop constructed!");

    using namespace polos::communication;

    Subscribe<WindowClose>([this](WindowClose&) {
        on_window_close();
    });

    Subscribe<EngineTerminate>([this](EngineTerminate&) {
        on_engine_terminate();
    });
}

void MainLoop::Run() const
{
    TimePoint start = utils::GetTimeNow();
#if !defined(ENABLE_PROFILING)
    // constexpr std::int32_t const kTargetFrames{120};
    // constexpr Duration const     kTimestep{1_sec / kTargetFrames};
#endif

    ecs::SystemRegistry& systems = ecs::SystemRegistry::Get();
    systems.Register([system = ecs::EditorCameraSystem{}](ecs::Scene* tScene, float tDeltaTime) mutable {
        system.Update(tScene, tDeltaTime, platform::PlatformManager::Get().GetMainWindow());
    });
    systems.Register([system = ecs::MeshInstanceSyncSystem{}](ecs::Scene* tScene, float) mutable {
        system.Update(tScene);
    });
    systems.Register([system = ecs::LightCollectionSystem{}](ecs::Scene* tScene, float) mutable {
        system.Update(tScene);
    });

    while (mIsRunning)
    {
        POLOS_PROFILE_ZONE("Engine frame");
        auto const current_time       = utils::GetTimeNow();
        auto const delta_time_in_secs = utils::ConvertToSeconds(current_time - start);
        start                         = current_time;

        {
            POLOS_PROFILE_ZONE("Engine update callbacks");
            communication::DispatchNow<communication::EngineUpdate>(delta_time_in_secs);
        }
        if (!mIsRunning)
        {
            break;
        }
        {
            POLOS_PROFILE_ZONE("ECS system updates");
            auto* scene = ecs::SceneManager::Get().GetCurrentScene();
            systems.UpdateAll(scene, delta_time_in_secs);
            if (scene != nullptr)
            {
                scene->ClearDirtyEntities();
            }
        }

        if (rendering::RenderingApi::Get().BeginFrame() != nullptr)
        {
            {
                POLOS_PROFILE_ZONE("Render update callbacks");
                communication::DispatchNow<communication::RenderUpdate>(delta_time_in_secs);
            }

            rendering::RenderingApi::Get().EndFrame();
            POLOS_PROFILE_FRAME();
            communication::DispatchNow<communication::EndFrame>();
        }

        {
            POLOS_PROFILE_ZONE("Deferred events and input");
            communication::DispatchDeferredEvents();
            communication::DispatchNow<communication::ProcessInput>();
        }


#if !defined(ENABLE_PROFILING)
        // auto const amount_time = utils::GetTimeNow() - start;
        // if (amount_time < kTimestep)
        // {
        //     POLOS_PROFILE_ZONE("120 FPS limiter sleep");
        //     std::this_thread::sleep_for(kTimestep - amount_time);
        // }
#endif
    }

#if !defined(ENABLE_PROFILING)
    // std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
}

void MainLoop::on_window_close()
{
    mIsRunning = false;
}

void MainLoop::on_engine_terminate()
{
    mIsRunning = false;
}

}// namespace polos::core
