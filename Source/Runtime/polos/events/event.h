#pragma once

#ifndef POLOS_EVENTS_EVENT_H
#define POLOS_EVENTS_EVENT_H

#include "polos/utils/type_util.h"
#include "polos/utils/alias.h"

namespace polos
{
    class BaseEvent
    {
    public:
        virtual ~BaseEvent() = default;
        virtual StringId _id() = 0;
    };

    template<typename T>
    class Event : public BaseEvent
    {
        StringId _id() override { return id; }//never going to be accessed, just for vtable
    public:
        inline static const StringId id = TypeHash<T>();
        
    };
}

#endif /* POLOS_EVENTS_EVENT_H */
