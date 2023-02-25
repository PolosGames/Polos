#pragma once

#include "polos/events/event.h"

namespace polos
{
    class window_focus : public Event<window_focus>
    {
    public:
        int32 is_focused;

        window_focus() = default;
        window_focus(int32 focused) : is_focused(focused) {}
    };
}
