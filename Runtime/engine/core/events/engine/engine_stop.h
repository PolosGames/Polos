#pragma once
#ifndef POLOS_CORE_EVENTS_ENGINE_ENGINESTOP_H_
#define POLOS_CORE_EVENTS_ENGINE_ENGINESTOP_H_

#include "events/event.h"
#include "utils/alias.h"

namespace polos
{
    class engine_stop : public event<engine_stop>
    {
    public:
        uint32 engine_event;

        engine_stop() = default;
        engine_stop(uint32 engine_event) 
            : engine_event(engine_event)
        {}
    };
}

#endif /* POLOS_CORE_EVENTS_ENGINE_ENGINESTOP_H_ */