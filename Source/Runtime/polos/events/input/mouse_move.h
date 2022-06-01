#pragma once

#ifndef POLOS_EVENTS_INPUT_MOUSEMOVE_H
#define POLOS_EVENTS_INPUT_MOUSEMOVE_H

#include "polos/events/event.h"

namespace polos
{
    class mouse_move : public Event<mouse_move>
    {
    public:
        float x, y;

        mouse_move() = default;
        mouse_move(float x, float y) : x(x), y(y) {}
    };
}

#endif /* POLOS_EVENTS_INPUT_MOUSEMOVE_H */
