#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct mouse_move final : public Event<mouse_move>
    {
        float x{};
        float y{};
    };
}
