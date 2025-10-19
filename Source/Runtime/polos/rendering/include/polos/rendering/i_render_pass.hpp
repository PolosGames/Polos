//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_PASS_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_PASS_HPP_

#include "polos/rendering/common.hpp"

namespace polos::rendering
{

class RenderPassResolver;
class RenderGraphRegistry;

class IRenderPass
{
public:
    virtual ~IRenderPass() = default;

    virtual void        Setup(RenderPassResolver& t_builder)                                      = 0;
    virtual void        Execute(VkCommandBuffer t_cmd_buf, const RenderGraphRegistry& t_registry) = 0;
    virtual char const* Name() const                                                              = 0;

    VkRenderPass vk_pass{VK_NULL_HANDLE};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_PASS_HPP_
