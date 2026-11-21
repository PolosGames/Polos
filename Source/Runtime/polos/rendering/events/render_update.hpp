///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_EVENTS_RENDER_UPDATE_HPP
#define POLOS_RENDERING_EVENTS_RENDER_UPDATE_HPP

#include "polos/communication/event.hpp"

#include <cmath>

namespace polos::rendering
{

struct RenderUpdate final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(RenderUpdate)

    explicit RenderUpdate(std::float_t tDeltaTime)
        : deltaTime{tDeltaTime}
    {}

    std::float_t deltaTime{0.0F};
};

}// namespace polos::rendering

DEFINE_EVENT_LOG_FORMAT(::polos::rendering::RenderUpdate, "Delta Time: {:.4f}", tEvent.deltaTime);

#endif// POLOS_RENDERING_EVENTS_RENDER_UPDATE_HPP
