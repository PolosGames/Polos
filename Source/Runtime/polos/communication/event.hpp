//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef CORE_COMMUNICATION_EVENT_H
#define CORE_COMMUNICATION_EVENT_H

#include <quill/DeferredFormatCodec.h>
#include <quill/bundled/fmt/ostream.h>

namespace polos::communication
{

/// Base struct for declaring new events.
///
/// Every event should inherit this struct and override the Name function.
struct base_event
{
    virtual ~base_event() = default;

    virtual char const* Name() = 0;
};

/// Utility function to quickly get a hash for an event based on the return string of Name()
/// @return hash of the event name based on FNV-1a hashing
template <typename Event>
std::int64_t EventHash()
{
    std::string const name_str{Event{}.Name()};
    constexpr std::int64_t k_FnvPrime{1099511628211LL};
    constexpr std::int64_t k_FnvOffset{14695981039346656037LL};
    std::int64_t hash = k_FnvOffset;
    for (char const c : name_str) {
        hash ^= c;
        hash *= k_FnvPrime;
    }
    return hash;
}

} // namespace polos::communication

/// Shall be called after each declaration of an event.
/// @param EventName Event's class name with its full namespace
/// @param Fmt Format string to log the event
/// @param __VA_ARGSS__ The variables that shall be logged. Each variable should start with "event." prefix
#define DEFINE_EVENT_LOG_FORMAT(EventName, Fmt, ...) \
    template <> \
    struct fmtquill::formatter<EventName> \
    { \
        constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); } \
        auto format(EventName const& event, format_context& ctx) const \
        { \
            return fmtquill::format_to(ctx.out(), Fmt, __VA_ARGS__); \
        } \
    }; \
    template <> \
    struct quill::Codec<EventName> : quill::DeferredFormatCodec<EventName> \
    { \
    };

/// Shall be called after each declaration of an event. The format should match as if overloading ostream.
/// @param EventName Event's class name with its full namespace
/// @param __VA_ARGSS__ The variables that shall be logged. Each variable should start with "event." prefix
#define DEFINE_EVENT_LOG_OSTREAM(EventName, ...) \
    std::ostream& operator<<(std::ostream& os, EventName const& event) \
    {\
        os << __VA_ARGS__;\
        return os;\
    } \
    template <> \
    struct fmtquill::formatter<EventName> : fmtquill::ostream_formatter \
    {}; \
    template <> \
    struct quill::Codec<EventName> : quill::DeferredFormatCodec<EventName> \
    { \
    };

#endif //CORE_COMMUNICATION_EVENT_H
