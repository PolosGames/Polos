///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_BEGIN_FRAME_HPP
#define POLOS_COMMUNICATION_BEGIN_FRAME_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct BeginFrame final : BaseEvent
{
    DECLARE_POLOS_EVENT(BeginFrame)
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_BEGIN_FRAME_HPP
