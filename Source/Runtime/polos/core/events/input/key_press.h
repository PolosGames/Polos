#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    class key_press final : public Event<key_press>
    {
    public:
        key_press() = default;
        explicit key_press(int32 p_Key) : key{p_Key} {}
    public:
        int32 key{};
    };
}
