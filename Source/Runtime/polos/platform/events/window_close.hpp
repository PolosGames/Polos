///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_EVENTS_WINDOW_CLOSE_HPP
#define POLOS_PLATFORM_EVENTS_WINDOW_CLOSE_HPP

#include "polos/communication/event.hpp"

namespace polos::platform
{

struct WindowClose final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(WindowClose);

    explicit WindowClose(void* tHandle)
        : windowHandle{tHandle}
    {}

    void* windowHandle{nullptr};
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_EVENTS_WINDOW_CLOSE_HPP
