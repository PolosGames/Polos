#pragma once

#include "polos/events/event.h"

namespace polos
{
    class key_press : public Event<key_press>
    {
    public:
        int32 key;

        key_press() = default;
        key_press(int32 key) : key(key) {}
    };
}
