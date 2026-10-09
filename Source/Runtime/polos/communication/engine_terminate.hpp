///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_ENGINE_TERMINATE_HPP
#define POLOS_COMMUNICATION_ENGINE_TERMINATE_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct EngineTerminate final : BaseEvent
{
    DECLARE_POLOS_EVENT(EngineTerminate)
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_ENGINE_TERMINATE_HPP
