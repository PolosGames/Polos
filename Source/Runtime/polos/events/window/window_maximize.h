#pragma once

#include "polos/events/event.h"

namespace polos
{
    class window_maximize : public Event<window_maximize>
    {
    public:
        int32 maximized;

        window_maximize() = default;
        window_maximize(int32 maximized) : maximized(maximized) {}
    };
}
