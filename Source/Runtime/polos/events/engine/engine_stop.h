#pragma once

#include "polos/events/event.h"
#include "polos/utils/alias.h"

namespace polos
{
    class engine_stop : public Event<engine_stop>
    {
    public:
        uint32 engine_event;

        engine_stop() = default;
        engine_stop(uint32 engine_event) 
            : engine_event(engine_event)
        {}
    };
}
