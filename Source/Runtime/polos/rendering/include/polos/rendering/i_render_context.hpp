///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_I_RENDER_CONTEXT_HPP
#define POLOS_RENDERING_I_RENDER_CONTEXT_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/module_macros.hpp"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace polos::rendering
{

class IRenderGraph;

class IRenderContext
{
public:
    virtual ~IRenderContext() = default;

    virtual auto Initialize(GLFWwindow* t_window) -> Result<void> = 0;
    virtual auto Shutdown() -> Result<void>                       = 0;

    virtual auto               BeginFrame() -> VkCommandBuffer         = 0;
    virtual auto               EndFrame() -> void                      = 0;
    [[nodiscard]] virtual auto GetRenderGraph() const -> IRenderGraph& = 0;
    [[nodiscard]] virtual auto IsInitialized() const -> bool           = 0;
};

}// namespace polos::rendering

extern "C"
{
    [[nodiscard]] RENDERING_EXPORT polos::rendering::IRenderContext* CreateRenderContext();
}

#endif// POLOS_RENDERING_I_RENDER_CONTEXT_HPP
