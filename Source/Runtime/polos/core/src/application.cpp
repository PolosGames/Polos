//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/application.hpp"

#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/logging/log_macros.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

namespace polos::core
{

Application::Application()
    : m_is_running{true}
{
    LogInfo("Application constructed!");

    communication::Subscribe<communication::engine_update>(
        [this](communication::engine_update& t_event)
        {
            this->on_engine_update(t_event);
        });
}

void Application::Run()
{
    using app_clock = std::chrono::high_resolution_clock;

    float        delta_time{0.0f};
    std::int64_t end{0};
    std::int64_t start = app_clock::now().time_since_epoch().count();

    // int count = 0;
    while (m_is_running)
    {
        end        = app_clock::now().time_since_epoch().count();
        delta_time = static_cast<float>(end - start) * 0.001f * 0.001f * 0.001f;

        start = app_clock::now().time_since_epoch().count();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        communication::Dispatch<communication::engine_update>(delta_time);
    }
}
void Application::on_engine_update(communication::engine_update& t_event)
{
    LogInfo("On Engine Update: [{}]", t_event);
}


}// namespace polos::core
