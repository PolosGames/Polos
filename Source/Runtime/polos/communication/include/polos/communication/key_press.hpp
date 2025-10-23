//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_KEY_PRESS_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_KEY_PRESS_HPP_

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct key_press final : base_event
{
    DECLARE_POLOS_EVENT(key_press);

    explicit key_press(std::int32_t const t_key);

    std::int32_t key{0};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::key_press, "Pressed Key: {}", event.key);


#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_KEY_PRESS_HPP_
