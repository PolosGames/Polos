#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct key_release final : public Event<key_release>
    {
        int32 key{};
    };
}
