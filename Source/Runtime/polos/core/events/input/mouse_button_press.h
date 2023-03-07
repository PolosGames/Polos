#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct mouse_button_press final : public Event<mouse_button_press>
    {
        int32 button{};
    };
}
