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

    Subscribe<window_close>([this](window_close&) {
        on_window_close();
    });

    Subscribe<engine_terminate>([this](engine_terminate&) {
        on_engine_terminate();
    });
}

void MainLoop::Run()
{
    using namespace std::literals::chrono_literals;

    Duration                     delta_time{Duration::zero()};
    TimePoint                    start = utils::GetTimeNow();
    Duration                     lag{Duration::zero()};
    constexpr std::int16_t const target_frames{60};
    constexpr Duration const     kTimestep{1_sec / target_frames};

    while (m_is_running)
    {
        auto current_time = utils::GetTimeNow();
        delta_time        = current_time - start;
        start             = current_time;

        lag += delta_time;

        std::float_t delta_time_in_secs = utils::ConvertToSeconds(delta_time);

        communication::DispatchNow<communication::engine_update>(delta_time_in_secs);
        lag -= kTimestep;

        rendering::RenderingApi::BeginFrame();
        communication::DispatchNow<communication::render_update>(delta_time_in_secs);

        rendering::RenderingApi::EndFrame();
        communication::DispatchNow<communication::end_frame>();

        communication::DispatchDeferredEvents();

#if defined(HOT_RELOAD)
        rendering::RenderingApi::ReloadIfNeeded();
#endif// HOT_RELOAD
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void MainLoop::on_window_close()
{
    m_is_running = false;
}

void MainLoop::on_engine_terminate()
{
    m_is_running = false;
}

}// namespace polos::core
