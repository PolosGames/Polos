#pragma once

#include "polos/events/event.h"

namespace polos
{
    class mouse_move : public Event<mouse_move>
    {
    public:
        float x, y;

        mouse_move() = default;
        mouse_move(float x, float y) : x(x), y(y) {}
    };
}
