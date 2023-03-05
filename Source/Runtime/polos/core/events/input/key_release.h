#pragma once

#include "polos/events/event.h"

namespace polos
{
    class key_release final : public Event<key_release>
    {
    public:
        key_release() = default;
        explicit key_release(int32 p_Key) : key{p_Key} {}
    public:
        int32 key{};
    };
}
