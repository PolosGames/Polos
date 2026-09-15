//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "engine_layer.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_press.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/communication/mouse_input.hpp"
#include "polos/communication/mouse_move.hpp"
#include "polos/communication/process_input.hpp"
#include "polos/core/input_state.hpp"
#include "polos/rendering/rendering_api.hpp"

#include <GLFW/glfw3.h>

#include <csignal>

namespace polos::core
{

namespace input
{
InputState g_input_state{};
}

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
    using namespace polos::communication;

    Subscribe<KeyPress>([](KeyPress& tEvent) {
        input::g_input_state.keysDown[static_cast<std::size_t>(tEvent.key)] = true;
    });

    Subscribe<KeyRelease>([](KeyRelease& tEvent) {
#if defined(HOT_RELOAD)
        if (tEvent.key == GLFW_KEY_R)
        {
            rendering::RenderingApi::DispatchReload();
        }
#endif// HOT_RELOAD

        input::g_input_state.keysDown[static_cast<std::size_t>(tEvent.key)] = false;
    });

    Subscribe<MouseMove>([](MouseMove& tEvent) {
        glm::vec2 new_pos               = {tEvent.mouseX, tEvent.mouseY};
        input::g_input_state.mouseDelta = new_pos - input::g_input_state.mousePos;
        input::g_input_state.mousePos   = new_pos;
    });

    Subscribe<MouseInput>([](MouseInput& tEvent) {
        input::g_input_state.keysDown[static_cast<std::size_t>(tEvent.button)] = tEvent.action == GLFW_PRESS;
    });

    Subscribe<ProcessInput>([](ProcessInput& tEvent) {
        std::ignore                       = tEvent;
        input::g_input_state.keysDownPrev = input::g_input_state.keysDown;
    });

    Subscribe<EndFrame>([](EndFrame&) {
        input::g_input_state.mouseDelta = glm::vec2(0.0F);
    });

    // Set up signal handlers for graceful shutdown
    std::ignore = std::signal(SIGINT, SignalHandler);
    std::ignore = std::signal(SIGTERM, SignalHandler);
    std::ignore = std::signal(SIGABRT, SignalHandler);
}

EngineLayer::~EngineLayer() = default;

}// namespace polos::core
