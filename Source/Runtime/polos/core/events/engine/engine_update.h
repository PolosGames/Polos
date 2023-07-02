#include "polos/core/events/event.h"

namespace polos
{
    struct engine_update : public Event<engine_update>
    {
        engine_update() = default;
        engine_update(float p_DeltaTime) : deltaTime{p_DeltaTime} {}

        float deltaTime;
    };
} // namespace polos