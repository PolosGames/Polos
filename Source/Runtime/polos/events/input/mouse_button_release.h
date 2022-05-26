#pragma once

#ifndef POLOS_EVENTS_INPUT_MOUSEBUTTONRELEASE_H
#define POLOS_EVENTS_INPUT_MOUSEBUTTONRELEASE_H

#include "polos/events/event.h"

namespace polos
{
    class mouse_button_release : public event<mouse_button_release>
    {
    public:
        int32 button;

        mouse_button_release() = default;
        mouse_button_release(int32 button) : button(button) {}
    };
}

#endif /* POLOS_EVENTS_INPUT_MOUSEBUTTONRELEASE_H */
