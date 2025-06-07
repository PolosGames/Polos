//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef CORE_COMMUNICATION_EVENTS_EVENT_H
#define CORE_COMMUNICATION_EVENTS_EVENT_H

#include "polos/communication/module_macros.hpp"

#include <quill/DeferredFormatCodec.h>
#include <quill/bundled/fmt/ostream.h>

namespace polos::communication
{

namespace detail
{

COMMUNICATION_EXPORT std::int64_t RetrieveEventHash(char const* t_name);

}

/// Base struct for declaring new events.
///
/// Every event should inherit this struct and override the Name function.
struct base_event
{
    virtual ~base_event() = default;
protected:
    virtual std::int64_t Id() = 0;
};

/// Utility function to quickly get a hash for an event based on the return string of Name()
/// @return hash of the event name based on FNV-1a hashing
template<typename Event>
std::int64_t EventHash()
{
    std::string const event_name = Event::Name();
    return detail::RetrieveEventHash(event_name.c_str());
}

}// namespace polos::communication

/// Declare and define a default constructor, override the Name function and add a friend declaration for
/// quill::DeferredFormatCodec. Has to be called inside event declaration scope. Don't forget to add ; to end of call.
/// @param EventName name of the event
#define DECLARE_POLOS_EVENT(EventName)                                                                                 \
private:                                                                                                               \
    friend struct quill::DeferredFormatCodec<EventName>;                                                               \
    std::int64_t Id() override                                                                                         \
    {                                                                                                                  \
        return 0;                                                                                                      \
    }                                                                                                                  \
public:                                                                                                                \
    EventName() = default;                                                                                             \
    static char const* Name()                                                                                          \
    {                                                                                                                  \
        return #EventName;                                                                                             \
    }


/// Declare and define log formatting of the event. Shall be called after each declaration of an event.
/// @param EventName Event's class name with its full namespace
/// @param Fmt Format string to log the event
/// @param __VA_ARGSS__ The variables that shall be logged. Each variable should start with "event." prefix
#define DEFINE_EVENT_LOG_FORMAT(EventName, Fmt, ...)                                                                   \
    template<>                                                                                                         \
    struct fmtquill::formatter<EventName>                                                                              \
    {                                                                                                                  \
        constexpr auto parse(format_parse_context& ctx)                                                                \
        {                                                                                                              \
            return ctx.begin();                                                                                        \
        }                                                                                                              \
        auto format(EventName const& event, format_context& ctx) const                                                 \
        {                                                                                                              \
            return fmtquill::format_to(ctx.out(), Fmt, __VA_ARGS__);                                                   \
        }                                                                                                              \
    };                                                                                                                 \
    template<>                                                                                                         \
    struct quill::Codec<EventName> : quill::DeferredFormatCodec<EventName>                                             \
    {                                                                                                                  \
    };

/// Shall be called after each declaration of an event. The format should match as if overloading ostream.
/// @param EventName Event's class name with its full namespace
/// @param __VA_ARGSS__ The variables that shall be logged. Each variable should start with "event." prefix
#define DEFINE_EVENT_LOG_OSTREAM(EventName, ...)                                                                       \
    std::ostream& operator<<(std::ostream& os, EventName const& event)                                                 \
    {                                                                                                                  \
        os << __VA_ARGS__;                                                                                             \
        return os;                                                                                                     \
    }                                                                                                                  \
    template<>                                                                                                         \
    struct fmtquill::formatter<EventName> : fmtquill::ostream_formatter                                                \
    {                                                                                                                  \
    };                                                                                                                 \
    template<>                                                                                                         \
    struct quill::Codec<EventName> : quill::DeferredFormatCodec<EventName>                                             \
    {                                                                                                                  \
    };

#endif//CORE_COMMUNICATION_EVENTS_EVENT_H
