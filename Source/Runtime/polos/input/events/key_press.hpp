///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_INPUT_EVENTS_KEY_PRESS_HPP
#define POLOS_INPUT_EVENTS_KEY_PRESS_HPP

#include "polos/communication/event.hpp"

namespace polos::input
{

struct KeyPress final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(KeyPress);

    explicit KeyPress(std::int32_t const tKey)
        : key{tKey}
    {}

    std::int32_t key{0};
};

}// namespace polos::input

DEFINE_EVENT_LOG_FORMAT(::polos::input::KeyPress, "Pressed Key: {}", tEvent.key);

#endif// POLOS_INPUT_EVENTS_KEY_PRESS_HPP
