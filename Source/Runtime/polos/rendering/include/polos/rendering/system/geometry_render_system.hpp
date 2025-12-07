///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_GEOMETRY_RENDER_SYSTEM_HPP
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_GEOMETRY_RENDER_SYSTEM_HPP

#include "polos/rendering/common.hpp"
#include "polos/rendering/i_render_system.hpp"

namespace polos::rendering
{

class GeometryRenderSystem : public IRenderSystem
{
public:
    GeometryRenderSystem(RenderContext& t_context, RenderGraph& t_graph);

    void Initialize() override;
    void Update() override;
private:
    RenderGraphResourceHandle m_sc_img_handle{RenderGraphResourceHandle::Invalid()};
    VkPipeline                m_basic_color_pipeline{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_GEOMETRY_RENDER_SYSTEM_HPP
