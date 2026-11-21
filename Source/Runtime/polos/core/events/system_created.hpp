///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_EVENTS_SYSTEM_CREATED_HPP
#define POLOS_CORE_EVENTS_SYSTEM_CREATED_HPP

#include "polos/communication/event.hpp"

namespace polos::core
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

struct SystemCreated final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(SystemCreated)

    explicit SystemCreated(SystemType tSystemType)
        : systemType{tSystemType}
    {}

    SystemType systemType;
};

}// namespace polos::core

DEFINE_EVENT_LOG_FORMAT(::polos::core::SystemCreated,
                        "System Created: {}",
                        ::polos::core::SystemTypeToString(tEvent.systemType));

#endif// POLOS_CORE_EVENTS_SYSTEM_CREATED_HPP
