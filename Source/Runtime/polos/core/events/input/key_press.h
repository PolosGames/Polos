#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct key_press final : public Event<key_press>
    {
        key_press() = default;
        explicit key_press(int32 p_Key) : key{p_Key} {}

        int32 key{};
    };
}
