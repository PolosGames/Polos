#pragma once

#ifndef POLOS_EVENTS_INPUT_MOUSEBUTTONPRESS_H
#define POLOS_EVENTS_INPUT_MOUSEBUTTONPRESS_H

#include "polos/events/event.h"

namespace polos
{
    class mouse_button_press : public event<mouse_button_press>
    {
    public:
        int32 button;

        mouse_button_press() = default;
        mouse_button_press(int32 button) : button(button) {}
    };
}

#endif /* POLOS_EVENTS_INPUT_MOUSEBUTTONPRESS_H */
