//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/app/application.hpp"

#include "polos/logging/logging_macros.hpp"

#include <chrono>
#include <cstdint>
#include <thread>
#include <iostream>

namespace polos::core
{

Application::Application()
    : m_is_running{true}
{
    LOG_APP_INFO("Application constructed!");
}

void Application::Run()
{
    using app_clock = std::chrono::high_resolution_clock;

    float delta_time{0.0f};
    std::int64_t end{0};
    std::int64_t start = app_clock::now().time_since_epoch().count();

    int count = 0;
    while (m_is_running)
    {
        end        = app_clock::now().time_since_epoch().count();
        delta_time = static_cast<float>(end - start) * 0.001f * 0.001f * 0.001f;

        start      = app_clock::now().time_since_epoch().count();

        LOG_APP_INFO("Running app... (count: {})", count++);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        static_cast<void>(delta_time);
    }
}


} // namespace polos::core
