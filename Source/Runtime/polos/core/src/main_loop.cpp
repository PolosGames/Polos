///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/main_loop.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/render_update.hpp"
#include "polos/communication/window_close.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/platform/window_manager.hpp"
#include "polos/utils/time.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>

namespace polos::core
{

MainLoop::MainLoop()
{
    LogInfo("MainLoop constructed!");

    communication::Subscribe<communication::window_close>([this](communication::window_close&) {
        on_window_close();
    });
}

void MainLoop::Run()
{
    using namespace std::literals::chrono_literals;

    std::thread update_thread{[this]() {
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

            while (lag >= kTimestep)
            {
                communication::Dispatch<communication::engine_update>(delta_time_in_secs);
                lag -= kTimestep;
            }
        }
    }};

    update_thread.detach();

    while (m_is_running)
    {
        communication::Dispatch<communication::render_update>(0.0f);

        communication::Dispatch<communication::end_frame>();
    }
}

void MainLoop::on_window_close()
{
    m_is_running = false;
}

}// namespace polos::core
