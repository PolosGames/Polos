///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/main_loop.hpp"
#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/module_reload.hpp"
#include "polos/communication/render_update.hpp"
#include "polos/communication/window_close.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/platform/window_manager.hpp"
#include "polos/rendering/render_context.hpp"
#include "polos/rendering/render_graph.hpp"
#include "polos/rendering/system/clear_screen_system.hpp"
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

    communication::Subscribe<communication::window_close>([this](communication::window_close&) {
        on_window_close();
    });

    communication::Subscribe<communication::engine_terminate>([this](communication::engine_terminate&) {
        on_engine_terminate();
    });

    communication::Subscribe<communication::module_reload>([this](polos::communication::module_reload& t_event) {
        m_rendering_module = &t_event.module;
    });

    m_render_systems.emplace_back(new rendering::ClearScreenSystem{});
}

void MainLoop::Run()
{
    using namespace std::literals::chrono_literals;

    // std::thread update_thread{[this]() {

    // }};

    // update_thread.detach();

    Duration                     delta_time{Duration::zero()};
    TimePoint                    start = utils::GetTimeNow();
    Duration                     lag{Duration::zero()};
    constexpr std::int16_t const target_frames{60};
    constexpr Duration const     kTimestep{1_sec / target_frames};

    auto* render_context = m_rendering_module->context;
    auto& render_graph   = render_context->GetRenderGraph();

    for (auto& system : m_render_systems) { system->Initialize(*render_context); }

    while (m_is_running)
    {
        auto current_time = utils::GetTimeNow();
        delta_time        = current_time - start;
        start             = current_time;

        lag += delta_time;

        std::float_t delta_time_in_secs = utils::ConvertToSeconds(delta_time);

        while (lag >= kTimestep)
        {
            communication::DispatchNow<communication::engine_update>(delta_time_in_secs);
            lag -= kTimestep;
        }

        auto cmd_buf = render_context->BeginFrame();

        communication::DispatchNow<communication::render_update>(delta_time_in_secs);

        for (auto const& system : m_render_systems) { system->Update(*render_context, render_graph); }

        render_graph.Compile();
        render_graph.Execute(cmd_buf);

        render_context->EndFrame();
        communication::DispatchNow<communication::end_frame>();

        render_graph.Clear();

        communication::DispatchDeferredEvents();
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
