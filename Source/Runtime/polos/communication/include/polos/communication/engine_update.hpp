///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_ENGINE_UPDATE_HPP
#define POLOS_COMMUNICATION_ENGINE_UPDATE_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct EngineUpdate final : BaseEvent
{
    DECLARE_POLOS_EVENT(EngineUpdate)

    explicit EngineUpdate(float t_delta_time)
        : delta_time{t_delta_time}
    {}

    float delta_time{0.0F};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::EngineUpdate, "Delta Time: {:.4f}", event.delta_time);

#endif// POLOS_COMMUNICATION_ENGINE_UPDATE_HPP
