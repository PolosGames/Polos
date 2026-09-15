///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_WINDOW_FOCUS_HPP
#define POLOS_COMMUNICATION_WINDOW_FOCUS_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct WindowFocus final : BaseEvent
{
    DECLARE_POLOS_EVENT(WindowFocus);

    explicit WindowFocus(std::int32_t tIsFocused)
        : isFocused{tIsFocused}
    {}

    std::int32_t isFocused{0U};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::WindowFocus, "Is Window Focused: {}", tEvent.isFocused);

#endif// POLOS_COMMUNICATION_WINDOW_FOCUS_HPP
