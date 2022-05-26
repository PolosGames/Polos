#pragma once

#ifndef POLOS_EVENTS_WINDOW_WINDOWFOCUS_H
#define POLOS_EVENTS_WINDOW_WINDOWFOCUS_H

#include "polos/events/event.h"

namespace polos
{
    class window_focus : public event<window_focus>
    {
    public:
        int32 is_focused;

        window_focus() = default;
        window_focus(int32 focused) : is_focused(focused) {}
    };
}

#endif /* POLOS_EVENTS_WINDOW_WINDOWFOCUS_H */
