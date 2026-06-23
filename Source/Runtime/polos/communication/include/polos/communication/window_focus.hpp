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

    explicit WindowFocus(std::int32_t t_is_focused)
        : is_focused{t_is_focused}
    {}

    std::int32_t is_focused{0U};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::WindowFocus, "Is Window Focused: {}", event.is_focused);

#endif// POLOS_COMMUNICATION_WINDOW_FOCUS_HPP
