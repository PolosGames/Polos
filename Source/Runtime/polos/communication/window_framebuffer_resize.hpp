///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_WINDOW_FRAMEBUFFER_RESIZE_HPP
#define POLOS_COMMUNICATION_WINDOW_FRAMEBUFFER_RESIZE_HPP

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct WindowFramebufferResize final : BaseEvent
{
    DECLARE_POLOS_EVENT(WindowFramebufferResize);

    explicit WindowFramebufferResize(std::int32_t tWidth, std::int32_t tHeight)
        : width{tWidth},
          height{tHeight}
    {}

    std::int32_t width{0U};
    std::int32_t height{0U};
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(::polos::communication::WindowFramebufferResize,
                        "New Framebuffer dimensions: {}, {}",
                        tEvent.width,
                        tEvent.height);

#endif// POLOS_COMMUNICATION_WINDOW_FRAMEBUFFER_RESIZE_HPP
