///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_EVENTS_WINDOW_FRAMEBUFFER_RESIZE_HPP
#define POLOS_PLATFORM_EVENTS_WINDOW_FRAMEBUFFER_RESIZE_HPP

#include "polos/communication/event.hpp"

namespace polos::platform
{

struct WindowFramebufferResize final : communication::BaseEvent
{
    DECLARE_POLOS_EVENT(WindowFramebufferResize);

    explicit WindowFramebufferResize(std::int32_t tWidth, std::int32_t tHeight)
        : width{tWidth},
          height{tHeight}
    {}

    std::int32_t width{0U};
    std::int32_t height{0U};
};

}// namespace polos::platform

DEFINE_EVENT_LOG_FORMAT(::polos::platform::WindowFramebufferResize,
                        "New Framebuffer dimensions: {}, {}",
                        tEvent.width,
                        tEvent.height);

#endif// POLOS_PLATFORM_EVENTS_WINDOW_FRAMEBUFFER_RESIZE_HPP
