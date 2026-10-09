///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_I_RENDER_CONTEXT_HPP
#define POLOS_RENDERING_I_RENDER_CONTEXT_HPP

#include "polos/platform/i_window_surface.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

namespace polos::ecs
{
struct SceneData;
}// namespace polos::ecs

namespace polos::rendering
{

class IRenderContext
{
public:
    virtual ~IRenderContext() = default;

    virtual auto Initialize(platform::IWindowSurface&) -> Result<void> = 0;
    virtual auto Shutdown() -> Result<void>                            = 0;

    virtual auto BeginFrame() -> VkCommandBuffer = 0;
    virtual auto EndFrame() -> void              = 0;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_I_RENDER_CONTEXT_HPP
