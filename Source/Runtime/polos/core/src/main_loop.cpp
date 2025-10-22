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
#include "polos/communication/rendering_module_reload.hpp"
#include "polos/communication/window_close.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/platform/platform_manager.hpp"
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
}

void MainLoop::Run()
{
    using namespace std::literals::chrono_literals;

    Duration                     delta_time{Duration::zero()};
    TimePoint                    start = utils::GetTimeNow();
    Duration                     lag{Duration::zero()};
    constexpr std::int16_t const target_frames{60};
    constexpr Duration const     kTimestep{1_sec / target_frames};

    auto& render_context = platform::PlatformManager::Instance().RenderingContextInstance();

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

        std::ignore = render_context.BeginFrame();
        communication::DispatchNow<communication::render_update>(delta_time_in_secs);

        render_context.EndFrame();
        communication::DispatchNow<communication::end_frame>();

        communication::DispatchDeferredEvents();
#if defined(HOT_RELOAD)
        platform::PlatformManager::Instance().CheckNeedModuleReload();
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
