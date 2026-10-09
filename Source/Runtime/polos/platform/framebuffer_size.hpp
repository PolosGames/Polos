///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_FRAMEBUFFER_SIZE_HPP
#define POLOS_PLATFORM_FRAMEBUFFER_SIZE_HPP

#include <cstdint>

namespace polos::platform
{

struct FramebufferSize
{
    std::uint32_t width{0U};
    std::uint32_t height{0U};
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_FRAMEBUFFER_SIZE_HPP
