#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_maximize final : public Event<window_maximize>
    {
        int32 maximized{};
    };
}
