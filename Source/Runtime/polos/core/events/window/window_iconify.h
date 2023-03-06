#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    class window_iconify final : public Event<window_iconify>
    {
    public:
        window_iconify() = default;
        window_iconify(int32 p_Iconified) : iconified{p_Iconified} {}
    public:
        int32 iconified{};
    };
}
