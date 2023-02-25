#pragma once

#include "polos/events/event.h"

namespace polos
{
    class window_position : public Event<window_position>
    {
    public:
        int32 x, y;

        window_position() = default;
        window_position(int32 x, int32 y) : x(x), y(y) {}
    };
}
