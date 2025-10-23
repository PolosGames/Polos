// //
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_pass/clear_screen_pass.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/render_graph_registry.hpp"
#include "polos/rendering/render_graph_resource_node.hpp"
#include "polos/rendering/render_pass_resolver.hpp"

namespace polos::rendering
{

ClearScreenPass::ClearScreenPass(RenderGraphResourceHandle t_handle)
    : m_sc_img_handle{t_handle}
{}

void ClearScreenPass::Setup(RenderPassResolver& t_builder)
{
    t_builder.Write(
        attachment_usage{
            .handle   = m_sc_img_handle,
            .load_op  = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .store_op = VK_ATTACHMENT_STORE_OP_STORE,
            // .clear_value = VkClearValue{.color{.float32 = {0.0f, 0.0f, 0.5f, 1.0f}}},
            .clear_value = kPolosClear,
        });
}

void ClearScreenPass::Execute(VkCommandBuffer, const RenderGraphRegistry&) {}

char const* ClearScreenPass::Name() const
{
    return "ClearScreenPass";
}

}// namespace polos::rendering
