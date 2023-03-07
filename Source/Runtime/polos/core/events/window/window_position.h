#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_position final : public Event<window_position>
    {
        int32 x{};
        int32 y{};
    };
}
