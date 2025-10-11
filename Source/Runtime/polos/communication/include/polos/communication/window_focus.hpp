///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_FOCUS_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_FOCUS_HPP_

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct window_focus final : base_event
{
    DECLARE_POLOS_EVENT(window_focus);

    explicit window_focus(std::int32_t t_is_focused)
        : is_focused{t_is_focused}
    {}

    std::int32_t is_focused;
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::window_focus, "Is Window Focused: {}", event.is_focused);

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_FOCUS_HPP_
