//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_CLEAR_SCREEN_SYSTEM_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_CLEAR_SCREEN_SYSTEM_HPP_

#include "polos/rendering/i_render_system.hpp"

namespace polos::rendering
{

class RenderContext;
class RenderGraph;
class VulkanSwapchain;

class ClearScreenSystem : public IRenderSystem
{
public:
    ClearScreenSystem();

    void Update(RenderGraphRegistry& t_registry) override;
private:
    RenderContext&   m_context;
    RenderGraph&     m_graph;
    VulkanSwapchain& m_swapchain;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_CLEAR_SCREEN_SYSTEM_HPP_
