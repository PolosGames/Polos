#pragma once

#ifndef POLOS_EVENTS_EVENT_H
#define POLOS_EVENTS_EVENT_H

#include "utils/type_util.h"
#include "utils/alias.h"

namespace polos
{
    class base_event
    {
        virtual StringId _id() = 0;
    };

    template<typename T>
    class event : public base_event
    {
        StringId _id() override { return id; }//never going to be accessed, just for vtable
    public:
        inline static const StringId id = TypeHash<T>();
        
        virtual ~event() = default;
    };
}

#endif /* POLOS_EVENTS_EVENT_H */
