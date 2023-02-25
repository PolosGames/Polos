#pragma once

#include "polos/utils/type_util.h"
#include "polos/utils/alias.h"

namespace polos
{
    template<typename T>
    StringId g_UniqueEventId = -1;

    class BaseEvent
    {
    public:
        virtual ~BaseEvent() = default;
    protected:
        virtual StringId id() = 0;
    };

    template<typename T>
    class Event : public BaseEvent
    {
    public:
        Event()
        {
            if (g_UniqueEventId<T> == -1)
            {
                g_UniqueEventId<T> = TypeHash<T>();
            }
        }
    private:
        StringId id() override { return g_UniqueEventId<T>; }//never going to be accessed, just for vtable
    };
}
