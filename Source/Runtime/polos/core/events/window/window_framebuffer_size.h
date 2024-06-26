#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_framebuffer_size final : public Event<window_framebuffer_size>
    {
        window_framebuffer_size() = default;
        explicit window_framebuffer_size(int32 p_Width, int32 p_Height) : width{p_Width}, height{p_Height} {}

        int32 width{};
        int32 height{};
    };
}
