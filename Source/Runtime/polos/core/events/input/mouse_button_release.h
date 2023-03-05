#pragma once

#include "polos/events/event.h"

namespace polos
{
    class mouse_button_release final : public Event<mouse_button_release>
    {
    public:
        mouse_button_release() = default;
        explicit mouse_button_release(int32 p_Button) : button{p_Button} {}
    public:
        int32 button{};
    };
}
