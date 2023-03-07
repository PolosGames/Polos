#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct mouse_button_release final : public Event<mouse_button_release>
    {
        mouse_button_release() = default;
        explicit mouse_button_release(int32 p_Button) : button{p_Button} {}

        int32 button{};
    };
}
