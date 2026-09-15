///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_MOUSE_MOVE_HPP
#define POLOS_COMMUNICATION_MOUSE_MOVE_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct MouseMove final : BaseEvent
{
    DECLARE_POLOS_EVENT(MouseMove)

    explicit MouseMove(std::double_t tMouseX, std::double_t tMouseY)
        : mouseX{tMouseX},
          mouseY{tMouseY}
    {}

    std::double_t mouseX{0.0};
    std::double_t mouseY{0.0};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::MouseMove, "Mouse x: {}, Mouse Y: {}", tEvent.mouseX, tEvent.mouseY);
#endif// POLOS_COMMUNICATION_MOUSE_MOVE_HPP
