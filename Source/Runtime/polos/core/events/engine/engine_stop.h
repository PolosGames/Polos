#pragma once

#include "polos/core/events/event.h"
#include "polos/utils/alias.h"

namespace polos
{
    class engine_stop : public Event<engine_stop>
    {
    public:
        engine_stop() = default;
    };
}