///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_I_RENDER_CONTEXT_HPP
#define POLOS_RENDERING_I_RENDER_CONTEXT_HPP

#include "polos/communication/error_code.hpp"
#include "polos/polos_api.hpp"
#include "polos/rendering/i_window_surface.hpp"
#include "polos/rendering/render_view.hpp"

#include <vulkan/vulkan.h>

namespace polos::rendering
{

struct RenderView;

class IRenderContext
{
public:
    virtual ~IRenderContext() = default;

    virtual auto Initialize(IWindowSurface&) -> Result<void> = 0;
    virtual auto Shutdown() -> Result<void>                  = 0;

    virtual auto               BeginFrame() -> VkCommandBuffer     = 0;
    virtual auto               EndFrame(RenderView const&) -> void = 0;
    [[nodiscard]] virtual auto IsInitialized() const -> bool       = 0;
};

}// namespace polos::rendering

extern "C"
{
    [[nodiscard]] POLOS_RENDERING_IMPL_API polos::rendering::IRenderContext* CreateRenderContext();
}

#endif// POLOS_RENDERING_I_RENDER_CONTEXT_HPP
