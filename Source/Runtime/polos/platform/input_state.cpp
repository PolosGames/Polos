///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/platform/input_state.hpp"

#include "polos/communication/end_frame.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_press.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/communication/mouse_input.hpp"
#include "polos/communication/mouse_move.hpp"
#include "polos/communication/process_input.hpp"

#include <GLFW/glfw3.h>

namespace polos::platform::input
{

InputState gInputState{};

auto Initialize() -> void
{
    using namespace polos::communication;

    Subscribe<KeyPress>([](KeyPress& tEvent) {
        gInputState.keysDown[static_cast<std::size_t>(tEvent.key)] = true;
    });

    Subscribe<KeyRelease>([](KeyRelease& tEvent) {
        gInputState.keysDown[static_cast<std::size_t>(tEvent.key)] = false;
    });

    Subscribe<MouseMove>([](MouseMove& tEvent) {
        glm::vec2 new_pos      = {tEvent.mouseX, tEvent.mouseY};
        gInputState.mouseDelta = new_pos - gInputState.mousePos;
        gInputState.mousePos   = new_pos;
    });

    Subscribe<MouseInput>([](MouseInput& tEvent) {
        gInputState.keysDown[static_cast<std::size_t>(tEvent.button)] = tEvent.action == GLFW_PRESS;
    });

    Subscribe<ProcessInput>([](ProcessInput& tEvent) {
        std::ignore              = tEvent;
        gInputState.keysDownPrev = gInputState.keysDown;
    });

    Subscribe<EndFrame>([](EndFrame&) {
        gInputState.mouseDelta = glm::vec2(0.0F);
    });
}

auto IsKeyDown(std::int32_t tKey) -> bool
{
    return gInputState.keysDown[static_cast<std::size_t>(tKey)];
}

auto IsKeyPressed(std::int32_t tKey) -> bool
{
    auto key_loc = static_cast<std::size_t>(tKey);
    return gInputState.keysDown[key_loc] && !gInputState.keysDownPrev[key_loc];
}

auto IsKeyReleased(std::int32_t tKey) -> bool
{
    auto key_loc = static_cast<std::size_t>(tKey);
    return !gInputState.keysDown[key_loc] && gInputState.keysDownPrev[key_loc];
}

}// namespace polos::platform::input
