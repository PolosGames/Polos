///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_EVENT_HPP
#define POLOS_COMMUNICATION_EVENT_HPP

#include "polos/polos_api.hpp"
#include "polos/utils/string_id.hpp"

#include <cstdint>
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace polos::communication
{

/// Base struct for declaring new events.
///
/// Every event should inherit this struct and override the Name function.

struct BaseEvent
{
    virtual ~BaseEvent() = default;
public:
    [[nodiscard]] virtual auto Hash() const -> utils::string_id = 0;
};

/// Utility function to quickly get a hash for an event based on the return string of Name()
/// @return hash of the event name based on FNV-1a hashing
template<typename Event>
auto EventHash() -> utils::string_id
{
    std::string const event_name = Event::Name();
    return utils::StrHash64(std::string_view{event_name});
}

}// namespace polos::communication

/// Define the default constructor, event name, and hash inside an event declaration.
/// @param EventName name of the event
#define DECLARE_POLOS_EVENT(EventName)                           \
private:                                                         \
    [[nodiscard]] auto Hash() const -> utils::string_id override \
    {                                                            \
        static constexpr utils::string_id kHash = []() {         \
            return polos::utils::StrHash64(#EventName);          \
        }();                                                     \
        return kHash;                                            \
    }                                                            \
public:                                                          \
    EventName() = default;                                       \
    [[nodiscard]] static constexpr auto Name() -> char const*    \
    { return #EventName; }


/// Define the event formatter after the event declaration. Refer to fields through tEvent.
#define DEFINE_EVENT_LOG_FORMAT(EventName, Fmt, ...)                                                    \
    template<>                                                                                         \
    struct std::formatter<EventName> : std::formatter<std::string_view>                                 \
    {                                                                                                  \
        auto format(EventName const& tEvent, std::format_context& tCtx) const                           \
        {                                                                                              \
            return std::formatter<std::string_view>::format(std::format(Fmt, __VA_ARGS__), tCtx);       \
        }                                                                                              \
    }

/// Define stream insertion and formatting for an event.
#define DEFINE_EVENT_LOG_OSTREAM(EventName, ...)                                                         \
    inline std::ostream& operator<<(std::ostream& tOs, EventName const& tEvent)                         \
    {                                                                                                  \
        return tOs << __VA_ARGS__;                                                                     \
    }                                                                                                  \
    template<>                                                                                         \
    struct std::formatter<EventName> : std::formatter<std::string_view>                                 \
    {                                                                                                  \
        auto format(EventName const& tEvent, std::format_context& tCtx) const                           \
        {                                                                                              \
            std::ostringstream output;                                                                 \
            operator<<(output, tEvent);                                                                \
            return std::formatter<std::string_view>::format(output.str(), tCtx);                       \
        }                                                                                              \
    }

#endif// POLOS_COMMUNICATION_EVENT_HPP
