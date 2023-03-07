#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct mouse_button_release final : public Event<mouse_button_release>
    {
        int32 button{};
    };
}
