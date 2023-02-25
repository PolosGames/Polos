#pragma once

#include "polos/events/event.h"

namespace polos
{
    class window_framebuffer_size final : public Event<window_framebuffer_size>
    {
    public:
        int32 width, height;

        window_framebuffer_size() = default;
        window_framebuffer_size(int32 width, int32 height) : width(width), height(height) {}
    };
}
