///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/input/input_state.hpp"

#include "polos/communication/event_bus.hpp"
#include "polos/core/events/end_frame.hpp"
#include "polos/input/events/key_press.hpp"
#include "polos/input/events/key_release.hpp"
#include "polos/input/events/mouse_input.hpp"
#include "polos/input/events/mouse_move.hpp"
#include "polos/input/events/process_input.hpp"

#include <GLFW/glfw3.h>
#include <cstddef>

namespace polos::input
{

InputState gInputState{};

namespace
{
auto isValidKey(std::int32_t tKey) -> bool
{
    return tKey >= 0 && static_cast<std::size_t>(tKey) < gInputState.keysDown.size();
}

void setKeyDown(std::int32_t tKey, bool tDown)
{
    if (!isValidKey(tKey))
    {
        return;
    }

    auto const index = static_cast<std::size_t>(tKey);
    if (gInputState.keysDown[index] == tDown)
    {
        return;
    }

    gInputState.keysDown[index] = tDown;
    if (tDown)
    {
        gInputState.keysPressed.set(index);
    }
    else
    {
        gInputState.keysReleased.set(index);
    }
}
}// namespace

auto Initialize() -> void
{
    using namespace polos::communication;

    Subscribe<KeyPress>([](KeyPress& tEvent) {
        setKeyDown(tEvent.key, true);
    });

    Subscribe<KeyRelease>([](KeyRelease& tEvent) {
        setKeyDown(tEvent.key, false);
    });

    Subscribe<MouseMove>([](MouseMove& tEvent) {
        glm::vec2 new_pos      = {tEvent.mouseX, tEvent.mouseY};
        gInputState.mouseDelta = new_pos - gInputState.mousePos;
        gInputState.mousePos   = new_pos;
    });

    Subscribe<MouseInput>([](MouseInput& tEvent) {
        setKeyDown(tEvent.button, tEvent.action == GLFW_PRESS);
    });

    Subscribe<ProcessInput>([](ProcessInput&) {
        gInputState.keysPressed.reset();
        gInputState.keysReleased.reset();
    });

    Subscribe<core::EndFrame>([](core::EndFrame&) {
        gInputState.mouseDelta = glm::vec2(0.0F);
    });
}

auto IsKeyDown(std::int32_t tKey) -> bool
{
    return isValidKey(tKey) && gInputState.keysDown[static_cast<std::size_t>(tKey)];
}

auto IsKeyPressed(std::int32_t tKey) -> bool
{
    return isValidKey(tKey) && gInputState.keysPressed[static_cast<std::size_t>(tKey)];
}

auto IsKeyReleased(std::int32_t tKey) -> bool
{
    return isValidKey(tKey) && gInputState.keysReleased[static_cast<std::size_t>(tKey)];
}

}// namespace polos::input
