#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    class window_resize final : public Event<window_resize>
    {
    public:
        window_resize() = default;
        explicit window_resize(int32 p_Width, int32 p_Height) : width{p_Width}, height{p_Height} {}
    public:
        int32 width{};
        int32 height{};
    };
}
