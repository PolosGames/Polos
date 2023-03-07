#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_focus final : public Event<window_focus>
    {
        int32 isFocused{};
    };
}
