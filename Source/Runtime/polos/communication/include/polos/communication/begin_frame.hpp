///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_BEGIN_FRAME_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_BEGIN_FRAME_HPP_

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct begin_frame final : base_event
{
    DECLARE_POLOS_EVENT(begin_frame)
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_BEGIN_FRAME_HPP_
