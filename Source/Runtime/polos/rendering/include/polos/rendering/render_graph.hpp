//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_HPP_

#include "polos/rendering/compiled_render_pass.hpp"
#include "polos/rendering/i_render_pass.hpp"
#include "polos/rendering/render_graph_resource_node.hpp"
#include "polos/rendering/vulkan_device.hpp"

#include <memory>
#include <vector>

namespace polos::rendering
{

struct render_graph_creation_details
{
    VulkanDevice* device;
};

class RenderGraph
{
public:
    RenderGraph()                         = default;
    ~RenderGraph()                        = default;
    RenderGraph(RenderGraph&&)            = delete;
    RenderGraph(RenderGraph&)             = delete;
    RenderGraph& operator=(RenderGraph&&) = delete;
    RenderGraph& operator=(RenderGraph&)  = delete;

    template<typename PassType, typename... Args>
        requires std::is_base_of_v<IRenderPass, PassType>
    auto AddRenderPass(Args&&... args) -> PassType&
    {
        m_passes.push_back(std::make_unique<PassType>(std::forward<Args>(args)...));
        return *static_cast<PassType*>(m_passes.back().get());
    }

    auto Compile() -> void;
    auto Execute(VkCommandBuffer t_cmd_buf) -> void;
    auto Clear() -> void;

    auto ImportTexture(std::string_view t_name, VkImage t_image) -> RendererResourceHandle;
private:
    std::vector<std::unique_ptr<IRenderPass>> m_passes;
    std::vector<render_graph_resource_node>   m_resources;
    std::vector<compiled_render_pass>         m_compiled_passes;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_HPP_
