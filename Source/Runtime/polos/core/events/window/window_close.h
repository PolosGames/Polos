#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct window_close final : public Event<window_close>
    {
        window_close() = default;
        explicit window_close(void* p_WinHandle) : winHandle{p_WinHandle} {}

        void* winHandle{};
    };
}

