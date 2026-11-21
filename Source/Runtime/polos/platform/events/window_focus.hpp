///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_EVENTS_WINDOW_FOCUS_HPP
#define POLOS_PLATFORM_EVENTS_WINDOW_FOCUS_HPP

#include "polos/communication/event.hpp"

namespace polos::platform
{

struct WindowFocus final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(WindowFocus);

    explicit WindowFocus(std::int32_t tIsFocused)
        : isFocused{tIsFocused}
    {}

    std::int32_t isFocused{0U};
};

}// namespace polos::platform

DEFINE_EVENT_LOG_FORMAT(::polos::platform::WindowFocus, "Is Window Focused: {}", tEvent.isFocused);

#endif// POLOS_PLATFORM_EVENTS_WINDOW_FOCUS_HPP
