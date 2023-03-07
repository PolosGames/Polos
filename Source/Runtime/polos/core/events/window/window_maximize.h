#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_maximize final : public Event<window_maximize>
    {
        window_maximize() = default;
        explicit window_maximize(int32 p_Maximized) : maximized{p_Maximized} {}

        int32 maximized{};
    };
}
