///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_PROCESS_INPUT_HPP
#define POLOS_COMMUNICATION_PROCESS_INPUT_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

/// Dispatched once per frame after this frame's KeyPress/KeyRelease events have been applied,
/// marking the point where input state should settle (e.g. snapshotting edge-detection state).
struct ProcessInput final : BaseEvent
{
    DECLARE_POLOS_EVENT(ProcessInput)
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_PROCESS_INPUT_HPP
