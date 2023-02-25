#pragma once

#include "polos/events/event.h"

namespace polos
{
    class mouse_button_release : public Event<mouse_button_release>
    {
    public:
        int32 button;

        mouse_button_release() = default;
        mouse_button_release(int32 button) : button(button) {}
    };
}
