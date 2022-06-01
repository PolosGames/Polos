#pragma once

#ifndef POLOS_EVENTS_WINDOW_WINDOWPOSITION_H
#define POLOS_EVENTS_WINDOW_WINDOWPOSITION_H

#include "polos/events/event.h"

namespace polos
{
    class window_position : public Event<window_position>
    {
    public:
        int32 x, y;

        window_position() = default;
        window_position(int32 x, int32 y) : x(x), y(y) {}
    };
}

#endif /* POLOS_EVENTS_WINDOW_WINDOWPOSITION_H */
