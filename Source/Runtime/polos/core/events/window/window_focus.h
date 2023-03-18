#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_focus final : public Event<window_focus>
    {
        window_focus() = default;
        explicit window_focus(int32 p_IsFocused) : isFocused{p_IsFocused} {}

        int32 isFocused;
    };
}
