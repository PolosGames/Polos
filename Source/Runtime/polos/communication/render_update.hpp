///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_RENDER_UPDATE_HPP
#define POLOS_COMMUNICATION_RENDER_UPDATE_HPP

#include "polos/communication/event.hpp"

#include <cmath>

namespace polos::communication
{

struct RenderUpdate final : BaseEvent
{
    DECLARE_POLOS_EVENT(RenderUpdate)

    explicit RenderUpdate(std::float_t tDeltaTime)
        : deltaTime{tDeltaTime}
    {}

    std::float_t deltaTime{0.0F};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::RenderUpdate, "Delta Time: {:.4f}", tEvent.deltaTime);

#endif// POLOS_COMMUNICATION_RENDER_UPDATE_HPP
