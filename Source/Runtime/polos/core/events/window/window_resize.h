#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_resize final : public Event<window_resize>
    {
        int32 width{};
        int32 height{};
    };
}
