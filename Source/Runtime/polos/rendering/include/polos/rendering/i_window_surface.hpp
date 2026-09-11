///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_I_WINDOW_SURFACE_HPP
#define POLOS_RENDERING_I_WINDOW_SURFACE_HPP

#include "polos/communication/error_code.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace polos::rendering
{

struct FramebufferSize
{
    std::uint32_t width{0U};
    std::uint32_t height{0U};
};

/// Everything the renderer needs from a window, with nothing about who made it.
/// The platform layer implements this, which is what keeps the windowing library
/// out of the renderer -- linked statically it would otherwise land in both this
/// library and the hot-reloadable one, each with its own copy of the global state.
class IWindowSurface
{
public:
    virtual ~IWindowSurface() = default;

    IWindowSurface(IWindowSurface const&)            = delete;
    IWindowSurface(IWindowSurface&&)                 = delete;
    IWindowSurface& operator=(IWindowSurface const&) = delete;
    IWindowSurface& operator=(IWindowSurface&&)      = delete;

    /// Instance extensions the windowing system needs, for VkInstanceCreateInfo.
    [[nodiscard]] virtual auto RequiredInstanceExtensions() const -> std::vector<char const*> = 0;

    [[nodiscard]] virtual auto CreateSurface(VkInstance t_instance) const -> Result<VkSurfaceKHR> = 0;

    /// Pixels, not screen coordinates -- they differ on hidpi displays.
    [[nodiscard]] virtual auto GetFramebufferSize() const -> FramebufferSize = 0;
protected:
    IWindowSurface() = default;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_I_WINDOW_SURFACE_HPP
