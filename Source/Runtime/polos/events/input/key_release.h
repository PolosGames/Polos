#pragma once

#include "polos/events/event.h"

namespace polos
{
    class key_release : public Event<key_release>
    {
    public:
        int32 key;

        key_release() = default;
        key_release(int32 key) : key(key) {}
    };
}
