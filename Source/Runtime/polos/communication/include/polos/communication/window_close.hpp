///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_WINDOW_CLOSE_HPP
#define POLOS_COMMUNICATION_WINDOW_CLOSE_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct WindowClose final : BaseEvent
{
    DECLARE_POLOS_EVENT(WindowClose);

    explicit WindowClose(void* t_handle)
        : window_handle{t_handle}
    {}

    void* window_handle{nullptr};
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_WINDOW_CLOSE_HPP
