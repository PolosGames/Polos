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

    explicit MouseMove(std::double_t t_mouse_x, std::double_t t_mouse_y)
        : mouse_x{t_mouse_x},
          mouse_y{t_mouse_y}
    {}

    std::double_t mouse_x{0.0};
    std::double_t mouse_y{0.0};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::MouseMove, "Mouse x: {}, Mouse Y: {}", event.mouse_x, event.mouse_y);
#endif// POLOS_COMMUNICATION_MOUSE_MOVE_HPP
