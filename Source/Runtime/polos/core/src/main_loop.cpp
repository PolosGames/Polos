//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/main_loop.hpp"

#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/utils/time.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

namespace polos::core
{

MainLoop::MainLoop()
{
    LogInfo("MainLoop constructed!");

    communication::Subscribe<communication::engine_update>(
        [this](communication::engine_update& t_event)
        {
            this->on_engine_update(t_event);
        });
}

void MainLoop::Run()
{
    using namespace std::literals::chrono_literals;

    Duration                     delta_time{0};
    TimePoint                    start = utils::GetTimeNow();
    Duration                     lag{0};
    constexpr std::int16_t const target_frames{60};
    constexpr Duration const     kTimestep{1_sec / target_frames};

    // int count = 0;
    while (m_is_running)
    {
        auto current_time = utils::GetTimeNow();
        delta_time        = current_time - start;
        start             = current_time;

        communication::Dispatch<communication::engine_update>(utils::ConvertToSeconds(delta_time));

        lag += delta_time;

        while (lag >= kTimestep) { lag -= kTimestep; }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void MainLoop::on_engine_update(communication::engine_update& t_event)
{
    LogInfo("On Engine Update: [{}]", t_event.delta_time);
}


}// namespace polos::core
