///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/main_loop.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/render_update.hpp"
#include "polos/communication/window_close.hpp"
#include "polos/logging/log_macros.hpp"
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
    Duration                     lag{Duration::zero()};
    constexpr std::int32_t const target_frames{120};
    constexpr Duration const     kTimestep{1_sec / target_frames};

    while (m_is_running)
    {
        auto const current_time = utils::GetTimeNow();
        delta_time              = current_time - start;

        //LogInfo("Frame Time: {} s", utils::ConvertToSeconds(delta_time));

        start = current_time;

        std::float_t const delta_time_in_secs = utils::ConvertToSeconds(delta_time);

        communication::DispatchNow<communication::EngineUpdate>(delta_time_in_secs);

        rendering::RenderingApi::BeginFrame();
        communication::DispatchNow<communication::RenderUpdate>(delta_time_in_secs);

        rendering::RenderingApi::EndFrame();
        communication::DispatchNow<communication::EndFrame>();

        communication::DispatchDeferredEvents();

#if defined(HOT_RELOAD)
        rendering::RenderingApi::ReloadIfNeeded();
#endif// HOT_RELOAD

        auto const amount_time = utils::GetTimeNow() - start;
        if (amount_time < kTimestep)
        {
            std::this_thread::sleep_for(kTimestep - amount_time);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void MainLoop::on_window_close()
{ m_is_running = false; }

void MainLoop::on_engine_terminate()
{ m_is_running = false; }

}// namespace polos::core
