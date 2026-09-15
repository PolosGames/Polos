///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_INTERFACE_I_WINDOW_SURFACE_HPP
#define POLOS_RENDERING_INTERFACE_I_WINDOW_SURFACE_HPP

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

class IWindowSurface
{
public:
    virtual ~IWindowSurface() = default;

    IWindowSurface(IWindowSurface const&)            = delete;
    IWindowSurface(IWindowSurface&&)                 = delete;
    IWindowSurface& operator=(IWindowSurface const&) = delete;
    IWindowSurface& operator=(IWindowSurface&&)      = delete;

    [[nodiscard]] virtual auto RequiredInstanceExtensions() const -> std::vector<char const*>     = 0;
    [[nodiscard]] virtual auto CreateSurface(VkInstance tInstance) const -> Result<VkSurfaceKHR>  = 0;
    [[nodiscard]] virtual auto GetFramebufferSize() const -> FramebufferSize                      = 0;
protected:
    IWindowSurface() = default;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INTERFACE_I_WINDOW_SURFACE_HPP
