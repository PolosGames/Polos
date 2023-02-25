#pragma once

#include "polos/events/event.h"

namespace polos
{
    class mouse_scroll : public Event<mouse_scroll>
    {
    public:
        float x_offset, y_offset;

        mouse_scroll() = default;
        mouse_scroll(float x_offset, float y_offset) : x_offset(x_offset), y_offset(y_offset) {}
    };
}
