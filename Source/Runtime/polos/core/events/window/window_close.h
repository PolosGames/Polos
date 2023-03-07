#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_close final : public Event<window_close>
    {
        void* winHandle{};
    };
}

