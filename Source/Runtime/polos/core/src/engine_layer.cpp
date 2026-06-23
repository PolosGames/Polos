//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/engine_layer.hpp"

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/rendering/rendering_api.hpp"

#include <GLFW/glfw3.h>

#include <csignal>

namespace polos::core
{

namespace
{

void OnKeyRelease(communication::KeyRelease const& t_event)
{
    std::ignore = t_event;
#if defined(HOT_RELOAD)
    if (t_event.key == GLFW_KEY_R)
    {
        rendering::RenderingApi::DispatchReload();
    }
#endif// HOT_RELOAD
}

void SignalHandler(int t_signal)
{
    std::ignore = t_signal;
    communication::DispatchNow<communication::EngineTerminate>();
}

}// namespace

EngineLayer::EngineLayer()
{
    communication::Subscribe<communication::KeyRelease>([](communication::KeyRelease& t_event) {
        OnKeyRelease(t_event);
    });

    // Set up signal handlers for graceful shutdown
    std::ignore = std::signal(SIGINT, SignalHandler);
    std::ignore = std::signal(SIGTERM, SignalHandler);
    std::ignore = std::signal(SIGABRT, SignalHandler);
}

EngineLayer::~EngineLayer() = default;

}// namespace polos::core
