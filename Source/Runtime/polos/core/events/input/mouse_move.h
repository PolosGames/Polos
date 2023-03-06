#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    class mouse_move final : public Event<mouse_move>
    {
    public:
        mouse_move() = default;
        explicit mouse_move(float p_X, float p_Y) : x{p_X}, y{p_Y} {}
    public:
        float x{};
        float y{};
    };
}
