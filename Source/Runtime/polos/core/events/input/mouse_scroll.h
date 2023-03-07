#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct mouse_scroll final : public Event<mouse_scroll>
    {
        float xOffset{};
        float yOffset{};
    };
}
