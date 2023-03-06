#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    class window_focus final : public Event<window_focus>
    {
    public:
        window_focus() = default;
        explicit window_focus(int32 p_IsFocused) : isFocused{p_IsFocused} {}
    public:
        int32 isFocused;
    };
}
