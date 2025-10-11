//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_pass/clear_screen_pass.hpp"

#include <cmath>

namespace polos::rendering
{

namespace
{
constexpr std::float_t const kPolosRed{0.50980395F};
constexpr std::float_t const kPolosGreen{0.59607846F};
constexpr std::float_t const kPolosBlue{0.6431373F};
}// namespace

ClearScreenPass::ClearScreenPass(RendererResourceHandle t_handle)
    : m_swapchain_image_handle{t_handle}
{}

void ClearScreenPass::Setup(RenderPassBuilder& t_builder)
{
    t_builder.Write({
        .handle = m_swapchain_image_handle, .load_op = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .store_op    = VK_ATTACHMENT_STORE_OP_STORE,
        .clear_value = VkClearValue{.color = {.float32 = {kPolosRed, kPolosGreen, kPolosBlue, 1.0f}}};
    });
}

void ClearScreenPass::Execute(VkCommandBuffer t_cmd_buf, const RenderPassRegistry& t_registry) {}

}// namespace polos::rendering
