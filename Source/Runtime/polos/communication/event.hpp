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

struct base_event
{
    virtual ~base_event() = default;

    virtual char const* Name() = 0;
};

template <typename Event>
std::int64_t EventHash()
{
    constexpr std::int64_t m = 1'000'000'009;
    constexpr std::int64_t p = 31;
    std::int64_t power_of_p = 1;
    std::int64_t hash_val = 0;

    // Loop to calculate the hash value
    // by iterating over the elements of string
    Event event;

    char const* name = event.Name();
    char c = 0;
    for (size_t i = 0; name[i] != '\0'; ++i)
    {
        c          = name[i];
        hash_val   = (hash_val + static_cast<std::int64_t>(c) * power_of_p) % m;
        power_of_p = (power_of_p * p) % m;
    }

    return (hash_val % m + m) % m;
}

} // namespace polos::communication

///
/// Shall be called after each declaration of an event.
///
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
