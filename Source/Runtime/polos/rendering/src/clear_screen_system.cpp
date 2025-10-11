//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/system/clear_screen_system.hpp"

#include "polos/rendering/render_context.hpp"
#include "polos/rendering/render_graph.hpp"
#include "polos/rendering/render_graph_registry.hpp"
#include "polos/rendering/render_pass/clear_screen_pass.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"

namespace polos::rendering
{

ClearScreenSystem::ClearScreenSystem()
    : m_context{rendering::RenderingInstance()},
      m_graph{m_context.GetRenderGraph()},
      m_swapchain{m_context.GetSwapchain()}
{}

void ClearScreenSystem::Update(RenderGraphRegistry&)
{
    m_graph.AddRenderPass<ClearScreenPass>(
        m_graph.ImportTexture("SwapchainImage", m_swapchain.GetCurrentImage(), m_swapchain.GetCurrentImageView()));
}

}// namespace polos::rendering
