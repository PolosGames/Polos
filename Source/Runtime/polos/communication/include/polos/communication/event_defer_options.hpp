//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_DEFER_OPTIONS_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_DEFER_OPTIONS_HPP_

#include <cstdint>

namespace polos::communication
{

enum class EventDeferOptions : std::uint8_t
{
    kImmediate,// Event is dispatched immediately when Dispatch is called.
    kNextFrame,// Event is queued and dispatched at the beginning of the next frame.
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_EVENT_DEFER_OPTIONS_HPP_
