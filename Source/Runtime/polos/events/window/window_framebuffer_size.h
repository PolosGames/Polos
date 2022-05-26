#pragma once

#ifndef POLOS_EVENTS_WINDOW_FRAMEBUFFERSIZE_H
#define POLOS_EVENTS_WINDOW_FRAMEBUFFERSIZE_H

#include "polos/events/event.h"

namespace polos
{
    class window_framebuffer_size final : public event<window_framebuffer_size>
    {
    public:
        int32 width, height;

        window_framebuffer_size() = default;
        window_framebuffer_size(int32 width, int32 height) : width(width), height(height) {}
    };
}

#endif /* POLOS_EVENTS_WINDOW_FRAMEBUFFERSIZE_H */
