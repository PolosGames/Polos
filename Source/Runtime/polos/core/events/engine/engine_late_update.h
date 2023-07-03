#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct engine_late_update final : public Event<engine_late_update>
    {
        engine_late_update() = default;
        explicit engine_late_update(float p_DeltaTime) : deltaTime{p_DeltaTime} {}

        float deltaTime;
    };
} // namespace polos