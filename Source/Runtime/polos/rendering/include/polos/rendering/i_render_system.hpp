///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_I_RENDER_SYSTEM_HPP
#define POLOS_RENDERING_I_RENDER_SYSTEM_HPP

#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/render_pass_layout_description.hpp"

namespace polos::rendering
{

class RenderContext;
class RenderGraph;

class IRenderSystem
{
public:
    IRenderSystem(RenderContext& t_context, RenderGraph& t_graph)
        : m_context{t_context},
          m_graph{t_graph}
    {}
    virtual ~IRenderSystem() = default;

    virtual void Initialize() = 0;
    virtual void Update()     = 0;
protected:
    RenderContext&                 m_context;
    RenderGraph&                   m_graph;
    render_pass_layout_description m_layout;
    VkRenderPass                   m_vk_render_pass{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_I_RENDER_SYSTEM_HPP
