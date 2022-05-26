#pragma once

#ifndef POLOS_EVENTS_CHARTYPE_H_
#define POLOS_EVENTS_CHARTYPE_H_

#include "polos/events/event.h"

namespace polos
{
    class char_type : public event<char_type>
    {
    public:
        uint32 unicode;

        char_type() = default;
        char_type(uint32 unicode) : unicode(unicode) {}
    };
}

#endif // POLOS_EVENTS_CHARTYPE_H_