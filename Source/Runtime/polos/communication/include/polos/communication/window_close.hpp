///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_CLOSE_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_CLOSE_HPP_

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct window_close final : base_event
{
    DECLARE_POLOS_EVENT(window_close);

    explicit window_close(void* t_handle)
        : window_handle{t_handle}
    {}

    void* window_handle;
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_CLOSE_HPP_
