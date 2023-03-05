#pragma once

#include "polos/events/event.h"

namespace polos
{
    class char_type final : public Event<char_type>
    {
    public:
        char_type() = default;
        explicit char_type(uint32 p_Unicode) : unicode{p_Unicode} {}
    public:
        uint32 unicode{};
    };
}
