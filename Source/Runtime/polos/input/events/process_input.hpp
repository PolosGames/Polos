///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_INPUT_EVENTS_PROCESS_INPUT_HPP
#define POLOS_INPUT_EVENTS_PROCESS_INPUT_HPP

#include "polos/communication/event.hpp"

namespace polos::input
{

/// Dispatched once per frame after this frame's KeyPress/KeyRelease events have been applied,
/// marking the point where input state should settle (e.g. snapshotting edge-detection state).
struct ProcessInput final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(ProcessInput)
};

}// namespace polos::input

#endif// POLOS_INPUT_EVENTS_PROCESS_INPUT_HPP
