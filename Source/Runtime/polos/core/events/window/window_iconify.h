#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_iconify final : public Event<window_iconify>
    {
        window_iconify() = default;
        window_iconify(int32 p_Iconified) : iconified{p_Iconified} {}

        int32 iconified{};
    };
}
