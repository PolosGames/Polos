///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_I_RENDER_PASS_HPP
#define POLOS_RENDERING_I_RENDER_PASS_HPP

#include <vulkan/vulkan.h>

namespace polos::rendering
{

class RenderPassResolver;
class RenderGraphRegistry;

class IRenderPass
{
public:
    virtual ~IRenderPass() = default;

    virtual auto               Setup(RenderPassResolver& t_builder) -> void                                      = 0;
    virtual auto               Execute(VkCommandBuffer t_cmd_buf, const RenderGraphRegistry& t_registry) -> void = 0;
    [[nodiscard]] virtual auto Name() const -> char const*                                                       = 0;

    VkRenderPass vk_pass{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_I_RENDER_PASS_HPP
