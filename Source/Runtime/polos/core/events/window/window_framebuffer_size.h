#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_framebuffer_size final : public Event<window_framebuffer_size>
    {
        int32 width{};
        int32 height{};
    };
}
