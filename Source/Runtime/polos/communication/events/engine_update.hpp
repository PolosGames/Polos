//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef COMMUNICATION_EVENTS_ENGINE_UPDATE_H
#define COMMUNICATION_EVENTS_ENGINE_UPDATE_H

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct engine_update final : base_event
{
    engine_update() = default;
    explicit engine_update(float const t_delta_time) : delta_time(t_delta_time) {}

    char const* Name() override { return "engine_update"; }

    float delta_time{0.0f};
private:
    friend struct quill::DeferredFormatCodec<engine_update>;
};

} // namespace polos::communication

// DEFINE_EVENT_LOG_FORMAT(
//     ::polos::communication::engine_update,
//     "Delta Time: {}", event.delta_time
// );

DEFINE_EVENT_LOG_OSTREAM(::polos::communication::engine_update, "Delta Time:" << event.delta_time)

#endif //COMMUNICATION_EVENTS_ENGINE_UPDATE_H
