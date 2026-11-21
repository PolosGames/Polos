///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_EVENTS_END_FRAME_HPP
#define POLOS_CORE_EVENTS_END_FRAME_HPP

#include "polos/communication/event.hpp"

namespace polos::core
{

struct EndFrame final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(EndFrame)
};

}// namespace polos::core

#endif// POLOS_CORE_EVENTS_END_FRAME_HPP
