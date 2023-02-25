#pragma once

#include "polos/events/event.h"

namespace polos
{
    class mouse_button_press : public Event<mouse_button_press>
    {
    public:
        int32 button;

        mouse_button_press() = default;
        mouse_button_press(int32 button) : button(button) {}
    };
}
