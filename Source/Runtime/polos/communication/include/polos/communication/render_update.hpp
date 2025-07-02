///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_RENDER_UPDATE_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_RENDER_UPDATE_HPP_

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct render_update final : base_event
{
    DECLARE_POLOS_EVENT(render_update)

    explicit render_update(float t_delta_time)
        : delta_time{t_delta_time}
    {}

    float delta_time{0.0f};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::render_update, "Delta Time: {:.4f}", event.delta_time);

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_RENDER_UPDATE_HPP_
