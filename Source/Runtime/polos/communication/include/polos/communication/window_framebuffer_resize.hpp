///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_FRAMEBUFFER_RESIZE_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_FRAMEBUFFER_RESIZE_HPP_

#include "polos/communication/event.hpp"

namespace polos::communication
{

struct window_framebuffer_resize final : base_event
{
    DECLARE_POLOS_EVENT(window_framebuffer_resize);

    explicit window_framebuffer_resize(std::int32_t t_width, std::int32_t t_height)
        : width{t_width},
          height{t_height}
    {}

    std::int32_t width;
    std::int32_t height;
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(
    ::polos::communication::window_framebuffer_resize,
    "New Framebuffer dimensions: {}, {}",
    event.width,
    event.height);

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_WINDOW_FRAMEBUFFER_RESIZE_HPP_
