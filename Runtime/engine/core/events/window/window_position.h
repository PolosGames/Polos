#pragma once

#ifndef POLOS_EVENTS_WINDOW_WINDOWPOSITION_H
#define POLOS_EVENTS_WINDOW_WINDOWPOSITION_H

#include "events/event.h"

namespace polos
{
    class window_position : public event<window_position>
    {
    public:
        int32 x, y;

        window_position() = default;
        window_position(int32 x, int32 y) : x(x), y(y) {}
    };
}

#endif /* POLOS_EVENTS_WINDOW_WINDOWPOSITION_H */
