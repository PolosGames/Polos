#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct mouse_button_press final : public Event<mouse_button_press>
    {
        mouse_button_press() = default;
        explicit mouse_button_press(int32 p_Button) : button{p_Button} {}

        int32 button{};
    };
}
