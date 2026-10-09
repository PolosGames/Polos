///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_I_WINDOW_SURFACE_HPP
#define POLOS_PLATFORM_I_WINDOW_SURFACE_HPP

#include "polos/platform/framebuffer_size.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace polos::platform
{

class IWindowSurface
{
public:
    virtual ~IWindowSurface() = default;

    IWindowSurface(IWindowSurface const&)            = delete;
    IWindowSurface(IWindowSurface&&)                 = delete;
    IWindowSurface& operator=(IWindowSurface const&) = delete;
    IWindowSurface& operator=(IWindowSurface&&)      = delete;

    [[nodiscard]] virtual auto RequiredInstanceExtensions() const -> std::vector<char const*>    = 0;
    /// @return The created surface, or VK_NULL_HANDLE on failure.
    [[nodiscard]] virtual auto CreateSurface(VkInstance tInstance) const -> VkSurfaceKHR         = 0;
    [[nodiscard]] virtual auto GetFramebufferSize() const -> FramebufferSize                     = 0;
protected:
    IWindowSurface() = default;
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_I_WINDOW_SURFACE_HPP
