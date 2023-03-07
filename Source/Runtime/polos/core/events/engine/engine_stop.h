#pragma once

#include "polos/core/events/event.h"
#include "polos/utils/alias.h"

namespace polos
{
    struct engine_stop : public Event<engine_stop>
    {
        engine_stop() = default;
    };
}
