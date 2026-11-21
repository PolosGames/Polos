///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_EVENTS_ENGINE_UPDATE_HPP
#define POLOS_CORE_EVENTS_ENGINE_UPDATE_HPP

#include "polos/communication/event.hpp"

namespace polos::core
{

struct EngineUpdate final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(EngineUpdate)

    explicit EngineUpdate(float tDeltaTime)
        : deltaTime{tDeltaTime}
    {}

    float deltaTime{0.0F};
};

}// namespace polos::core

DEFINE_EVENT_LOG_FORMAT(::polos::core::EngineUpdate, "Delta Time: {:.4f}", tEvent.deltaTime);

#endif// POLOS_CORE_EVENTS_ENGINE_UPDATE_HPP
