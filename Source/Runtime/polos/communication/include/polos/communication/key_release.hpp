///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_KEY_RELEASE_HPP
#define POLOS_COMMUNICATION_KEY_RELEASE_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct key_release final : base_event// NOLINT
{
    DECLARE_POLOS_EVENT(key_release);

    explicit key_release(std::int32_t t_key)
        : key{t_key}
    {}

    std::int32_t key{0};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::key_release, "Released Key: {}", event.key);

#endif// POLOS_COMMUNICATION_KEY_RELEASE_HPP
