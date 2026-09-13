//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/engine_layer.hpp"

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

void SignalHandler(int t_signal)
{
    std::ignore = t_signal;
    communication::DispatchNow<communication::EngineTerminate>();
}

}// namespace

EngineLayer::EngineLayer()
{
    using namespace polos::communication;

    Subscribe<KeyPress>([](KeyPress& t_event) {
        input::g_input_state.keys_down[static_cast<std::size_t>(t_event.key)] = true;
    });

    Subscribe<KeyRelease>([](KeyRelease& t_event) {
#if defined(HOT_RELOAD)
        if (t_event.key == GLFW_KEY_R)
        {
            rendering::RenderingApi::DispatchReload();
        }
#endif// HOT_RELOAD

        input::g_input_state.keys_down[static_cast<std::size_t>(t_event.key)] = false;
    });

    Subscribe<MouseMove>([](MouseMove& t_event) {
        glm::vec2 new_pos                = {t_event.mouse_x, t_event.mouse_y};
        input::g_input_state.mouse_delta = new_pos - input::g_input_state.mouse_pos;
        input::g_input_state.mouse_pos   = new_pos;
    });

    Subscribe<MouseInput>([](MouseInput& t_event) {
        input::g_input_state.keys_down[static_cast<std::size_t>(t_event.button)] = t_event.action == GLFW_PRESS;
    });

    Subscribe<ProcessInput>([](ProcessInput& t_event) {
        std::ignore                         = t_event;
        input::g_input_state.keys_down_prev = input::g_input_state.keys_down;
    });

    Subscribe<EndFrame>([](EndFrame&) {
        input::g_input_state.mouse_delta = glm::vec2(0.0F);
    });

    // Set up signal handlers for graceful shutdown
    std::ignore = std::signal(SIGINT, SignalHandler);
    std::ignore = std::signal(SIGTERM, SignalHandler);
    std::ignore = std::signal(SIGABRT, SignalHandler);
}

EngineLayer::~EngineLayer() = default;

}// namespace polos::core
