#pragma once

#include "polos/events/event.h"

namespace polos
{
    class mouse_scroll final : public Event<mouse_scroll>
    {
    public:
        mouse_scroll() = default;
        explicit mouse_scroll(float p_XOffset, float p_YOffset) : xOffset{p_XOffset}, yOffset{p_YOffset} {}
    public:
        float xOffset{};
        float yOffset{};
    };
}
