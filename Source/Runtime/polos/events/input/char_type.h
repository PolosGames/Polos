#pragma once

#include "polos/events/event.h"

namespace polos
{
    class char_type : public Event<char_type>
    {
    public:
        uint32 unicode;

        char_type() = default;
        char_type(uint32 ucode) : unicode{ucode} {}
    };
}
