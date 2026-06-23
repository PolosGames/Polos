///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_KEY_PRESS_HPP
#define POLOS_COMMUNICATION_KEY_PRESS_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct KeyPress final : BaseEvent
{
    DECLARE_POLOS_EVENT(KeyPress);

    explicit KeyPress(std::int32_t const t_key);

    std::int32_t key{0};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::KeyPress, "Pressed Key: {}", event.key);

#endif// POLOS_COMMUNICATION_KEY_PRESS_HPP
