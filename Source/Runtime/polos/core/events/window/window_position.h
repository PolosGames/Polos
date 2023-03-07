#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_position final : public Event<window_position>
    {
        window_position() = default;
        explicit window_position(int32 p_X, int32 p_Y) : x{p_X}, y{p_Y} {}

        int32 x{};
        int32 y{};
    };
}
