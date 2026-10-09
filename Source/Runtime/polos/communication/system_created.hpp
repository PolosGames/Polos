///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_SYSTEM_CREATED_HPP
#define POLOS_COMMUNICATION_SYSTEM_CREATED_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

enum class SystemType : std::uint8_t
{
    kWindowing,
    kRendering,
    kSystemTypeMax,
};

constexpr std::string_view SystemTypeToString(SystemType tType)
{
    switch (tType)
    {
        case SystemType::kWindowing: return "Windowing";
        case SystemType::kRendering: return "Rendering";
        default: return "Unknown";
    }
}

struct SystemCreated final : BaseEvent
{
    DECLARE_POLOS_EVENT(SystemCreated)

    explicit SystemCreated(SystemType tSystemType)
        : systemType{tSystemType}
    {}

    SystemType systemType;
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::SystemCreated,
                        "System Created: {}",
                        ::polos::communication::SystemTypeToString(tEvent.systemType));

#endif// POLOS_COMMUNICATION_SYSTEM_CREATED_HPP
