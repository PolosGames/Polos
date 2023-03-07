#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct key_press final : public Event<key_press>
    {
        int32 key{};
    };
}
