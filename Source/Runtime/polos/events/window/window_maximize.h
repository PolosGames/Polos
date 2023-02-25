#pragma once

#include "polos/events/event.h"

namespace polos
{
    class window_maximize final : public Event<window_maximize>
    {
    public:
        window_maximize() = default;
        explicit window_maximize(int32 p_Maximized) : maximized{p_Maximized} {}
    public:
        int32 maximized{};
    };
}
