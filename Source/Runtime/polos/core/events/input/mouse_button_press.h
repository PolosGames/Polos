#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    class mouse_button_press final : public Event<mouse_button_press>
    {
    public:
        mouse_button_press() = default;
        explicit mouse_button_press(int32 p_Button) : button{p_Button} {}
    public:
        int32 button{};
    };
}
