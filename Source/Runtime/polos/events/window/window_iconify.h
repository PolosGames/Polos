#pragma once

#include "polos/events/event.h"

namespace polos
{
    class window_iconify : public Event<window_iconify>
    {
    public:
        int32 iconified;

        window_iconify() = default;
        window_iconify(int32 iconified) : iconified(iconified) {}
    };
}
