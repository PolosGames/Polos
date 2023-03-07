#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct char_type final : public Event<char_type>
    {
        uint32 unicode{};
    };
}
