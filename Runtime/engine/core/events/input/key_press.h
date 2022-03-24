#pragma once

#ifndef POLOS_EVENTS_INPUT_KEYPRESS_H
#define POLOS_EVENTS_INPUT_KEYPRESS_H

#include "events/event.h"

namespace polos
{
    class key_press : public event<key_press>
    {
    public:
        int32 key;

        key_press() = default;
        key_press(int32 key) : key(key) {}
    };
}

#endif /* POLOS_EVENTS_INPUT_KEYPRESS_H */
