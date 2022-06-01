#pragma once

#ifndef POLOS_EVENTS_WINDOW_WINDOWRESIZE_H
#define POLOS_EVENTS_WINDOW_WINDOWRESIZE_H

#include "polos/events/event.h"

namespace polos
{
    class window_resize : public Event<window_resize>
    {
    public:
        int32 width, height;

        window_resize() = default;
        window_resize(int32 width, int32 height) : width(width), height(height) {}
    };
}

#endif /* POLOS_EVENTS_WINDOW_WINDOWRESIZE_H */
