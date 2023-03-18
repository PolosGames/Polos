#pragma once

#include "polos/core/events/event.h"

namespace polos
{
    struct char_type final : public Event<char_type>
    {
        char_type() = default;
        explicit char_type(uint32 p_Unicode) : unicode{p_Unicode} {}

        uint32 unicode{};
    };
}
