#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct key_release final : public Event<key_release>
    {
        key_release() = default;
        explicit key_release(int32 p_Key) : key{p_Key} {}
        
        int32 key{};
    };
}
