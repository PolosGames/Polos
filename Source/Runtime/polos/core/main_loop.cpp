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
#include "polos/ecs/systems/dirty_transform_system.hpp"
#include "polos/ecs/systems/editor_camera_system.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/utils/time.hpp"

#include <quill/std/Chrono.h>

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
    using namespace std::literals::chrono_literals;

    Duration                     delta_time{Duration::zero()};
    TimePoint                    start = utils::GetTimeNow();
    constexpr std::int32_t const kTargetFrames{120};
    constexpr Duration const     kTimestep{1_sec / kTargetFrames};

    ecs::EditorCameraSystem editorCameraSystem;
    ecs::SystemRegistry systems;
    systems.Register([&editorCameraSystem](ecs::Scene* tScene, float tDeltaTime) {
        editorCameraSystem.Update(tScene, tDeltaTime, platform::PlatformManager::Instance().GetMainWindow());
    });
    systems.Register([system = ecs::DirtyTransformSystem{}](ecs::Scene* tScene, float) mutable {
        system.Update(tScene);
    });

    while (mIsRunning)
    {
        auto const current_time = utils::GetTimeNow();
        delta_time              = current_time - start;

        //LogInfo("Frame Time: {} s", utils::ConvertToSeconds(delta_time));

        start = current_time;

        std::float_t const delta_time_in_secs = utils::ConvertToSeconds(delta_time);

        communication::DispatchNow<communication::EngineUpdate>(delta_time_in_secs);
        auto* scene = ecs::SceneManager::GetCurrentScene();
        systems.UpdateAll(scene, delta_time_in_secs);
        if (scene != nullptr)
        {
            scene->ClearDirtyEntities();
        }

        if (VK_NULL_HANDLE != rendering::RenderingApi::BeginFrame())
        {
            communication::DispatchNow<communication::RenderUpdate>(delta_time_in_secs);

            rendering::RenderingApi::EndFrame();
            communication::DispatchNow<communication::EndFrame>();
        }

        communication::DispatchDeferredEvents();
        communication::DispatchNow<communication::ProcessInput>();


        auto const amount_time = utils::GetTimeNow() - start;
        if (amount_time < kTimestep)
        {
            std::this_thread::sleep_for(kTimestep - amount_time);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
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
