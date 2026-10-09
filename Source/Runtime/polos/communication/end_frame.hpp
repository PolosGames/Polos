///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_END_FRAME_HPP
#define POLOS_COMMUNICATION_END_FRAME_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct EndFrame final : BaseEvent
{
    DECLARE_POLOS_EVENT(EndFrame)
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_END_FRAME_HPP
