#pragma once

#ifndef POLOS_EVENTS_WINDOW_WINDOWCLOSE_H
#define POLOS_EVENTS_WINDOW_WINDOWCLOSE_H

#include "events/event.h"

namespace polos
{
    class window_close final : public event<window_close>
    {
    public:
        window_close() = default;
    };
}

#endif /* POLOS_EVENTS_WINDOW_WINDOWCLOSE_H */
