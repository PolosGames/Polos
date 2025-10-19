//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_SYSTEM_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_SYSTEM_HPP_

#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/render_pass_layout_description.hpp"

namespace polos::rendering
{

class RenderContext;
class RenderGraph;

class RENDERING_EXPORT IRenderSystem
{
public:
    virtual ~IRenderSystem() = default;

    virtual void Initialize(RenderContext& t_context)                   = 0;
    virtual void Update(RenderContext& t_context, RenderGraph& t_graph) = 0;
protected:
    render_pass_layout_description m_layout;
    VkRenderPass                   m_vk_render_pass{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_SYSTEM_HPP_
