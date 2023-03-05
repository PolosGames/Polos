#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    class window_close final : public Event<window_close>
    {
    public:
        window_close() = default;
        explicit window_close(void* p_WinHandle) : winHandle{p_WinHandle} {}
    public:
        void* winHandle{};
    };
}

