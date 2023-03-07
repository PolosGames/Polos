#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_iconify final : public Event<window_iconify>
    {
        int32 iconified{};
    };
}
