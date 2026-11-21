///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_INPUT_EVENTS_KEY_RELEASE_HPP
#define POLOS_INPUT_EVENTS_KEY_RELEASE_HPP

#include "polos/communication/event.hpp"

namespace polos::input
{

struct KeyRelease final : communication::BaseEvent// NOLINT
{
    DECLARE_POLOS_EVENT(KeyRelease);

    explicit KeyRelease(std::int32_t tKey)
        : key{tKey}
    {}

    std::int32_t key{0};
};

}// namespace polos::input

DEFINE_EVENT_LOG_FORMAT(::polos::input::KeyRelease, "Released Key: {}", tEvent.key);

#endif// POLOS_INPUT_EVENTS_KEY_RELEASE_HPP
