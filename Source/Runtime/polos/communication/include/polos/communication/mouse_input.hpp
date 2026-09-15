///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_MOUSE_INPUT_HPP
#define POLOS_COMMUNICATION_MOUSE_INPUT_HPP

#include "polos/communication/event.hpp"

#include <cstdint>

namespace polos::communication
{

struct MouseInput final : BaseEvent
{
    DECLARE_POLOS_EVENT(MouseInput)

    explicit MouseInput(std::int32_t tButton, std::int32_t tAction)
        : button{tButton},
          action{tAction}
    {}

    std::int32_t button{0};
    std::int32_t action{0};
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_MOUSE_INPUT_HPP
