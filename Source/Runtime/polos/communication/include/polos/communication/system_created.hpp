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

constexpr std::string_view SystemTypeToString(SystemType t_type)
{
    switch (t_type)
    {
        case SystemType::kWindowing: return "Windowing";
        case SystemType::kRendering: return "Rendering";
        default: return "Unknown";
    }
}

struct system_created final : base_event
{
    DECLARE_POLOS_EVENT(system_created)

    explicit system_created(SystemType t_system_type)
        : system_type{t_system_type}
    {}

    SystemType system_type;
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(
    ::polos::communication::system_created,
    "System Created: {}",
    ::polos::communication::SystemTypeToString(event.system_type));

#endif// POLOS_COMMUNICATION_SYSTEM_CREATED_HPP
