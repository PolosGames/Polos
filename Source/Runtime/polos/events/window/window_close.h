#pragma once

#include "polos/events/event.h"

namespace polos
{
    class window_close final : public Event<window_close>
    {
    public:
        window_close(void* win_handle) : win_handle{win_handle} {}
        window_close() : win_handle{nullptr} {}

    public:
        void* win_handle;
    };
}

