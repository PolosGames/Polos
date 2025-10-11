//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_PASS_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_PASS_HPP_

#include "polos/rendering/render_graph_registry.hpp"
#include "polos/rendering/render_pass_builder.hpp"

namespace polos::rendering
{
class IRenderPass
{
public:
    virtual ~IRenderPass() = default;

    virtual void Setup(RenderPassBuilder& t_builder)                                       = 0;
    virtual void Execute(VkCommandBuffer t_cmd_buf, const RenderGraphRegistry& t_registry) = 0;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_PASS_HPP_
