//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/engine_layer.hpp"

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/platform/input_state.hpp"

#include <csignal>

namespace polos::core
{

namespace
{

void SignalHandler(int tSignal)
{
    std::ignore = tSignal;
    communication::DispatchNow<communication::EngineTerminate>();
}

}// namespace

EngineLayer::EngineLayer()
{
    platform::input::Initialize();

    // Set up signal handlers for graceful shutdown
    std::ignore = std::signal(SIGINT, SignalHandler);
    std::ignore = std::signal(SIGTERM, SignalHandler);
    std::ignore = std::signal(SIGABRT, SignalHandler);
}

EngineLayer::~EngineLayer() = default;

}// namespace polos::core
