#pragma once

#ifndef POLOS_EVENTS_EVENT_H
#define POLOS_EVENTS_EVENT_H

#include "utils/type_util.h"
#include "utils/alias.h"

namespace polos
{
    class base_event
    {
        virtual event_id _id() = 0;
    };

    template<typename T>
    class event : public base_event
    {
        event_id _id() override { return id; } //never going to be accessed, just for vtable
    public:
        inline static const event_id id = TypeHash<T>();
        
        virtual ~event() = default;
    };
}

#endif /* POLOS_EVENTS_EVENT_H */
