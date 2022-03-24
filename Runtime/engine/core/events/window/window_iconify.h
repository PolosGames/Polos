#pragma once

#ifndef POLOS_EVENTS_WINDOW_WINDOWICONIFY_H
#define POLOS_EVENTS_WINDOW_WINDOWICONIFY_H

#include "events/event.h"

namespace polos
{
    class window_iconify : public event<window_iconify>
    {
    public:
        int32 iconified;

        window_iconify() = default;
        window_iconify(int32 iconified) : iconified(iconified) {}
    };
}

#endif /* POLOS_EVENTS_WINDOW_WINDOWICONIFY_H */
