///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_INPUT_EVENTS_MOUSE_MOVE_HPP
#define POLOS_INPUT_EVENTS_MOUSE_MOVE_HPP

#include "polos/communication/event.hpp"

#include <cmath>

namespace polos::input
{

struct MouseMove final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(MouseMove)

    explicit MouseMove(std::double_t tMouseX, std::double_t tMouseY)
        : mouseX{tMouseX},
          mouseY{tMouseY}
    {}

    std::double_t mouseX{0.0};
    std::double_t mouseY{0.0};
};

}// namespace polos::input

DEFINE_EVENT_LOG_FORMAT(::polos::input::MouseMove, "Mouse x: {}, Mouse Y: {}", tEvent.mouseX, tEvent.mouseY);
#endif// POLOS_INPUT_EVENTS_MOUSE_MOVE_HPP
