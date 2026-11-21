///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_INPUT_EVENTS_MOUSE_INPUT_HPP
#define POLOS_INPUT_EVENTS_MOUSE_INPUT_HPP

#include "polos/communication/event.hpp"

#include <cstdint>

namespace polos::input
{

struct MouseInput final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(MouseInput)

    explicit MouseInput(std::int32_t tButton, std::int32_t tAction)
        : button{tButton},
          action{tAction}
    {}

    std::int32_t button{0};
    std::int32_t action{0};
};

}// namespace polos::input

DEFINE_EVENT_LOG_FORMAT(::polos::input::MouseInput, "Mouse x: {}, Mouse Y: {}", tEvent.button, tEvent.action);

#endif// POLOS_INPUT_EVENTS_MOUSE_INPUT_HPP
