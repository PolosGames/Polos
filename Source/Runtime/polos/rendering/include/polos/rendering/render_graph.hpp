//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_HPP_

#include "polos/rendering/compiled_render_pass.hpp"
#include "polos/rendering/vulkan_device.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace polos::rendering
{

struct render_graph_creation_details
{
    VkDevice device;
};

struct render_graph_resource_node;
class IRenderPass;

class RenderGraph
{
public:
    RenderGraph();
    ~RenderGraph();
    RenderGraph(RenderGraph&&)            = delete;
    RenderGraph(RenderGraph&)             = delete;
    RenderGraph& operator=(RenderGraph&&) = delete;
    RenderGraph& operator=(RenderGraph&)  = delete;

    auto Create(render_graph_creation_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    template<typename PassType, typename... Args>
        requires std::is_base_of_v<IRenderPass, PassType>
    auto AddRenderPass(Args&&... args) -> void
    {
        m_passes.push_back(std::make_unique<PassType>(std::forward<Args>(args)...));
    }

    /// @brief Creates actual VkRenderPass objects from Layouts of IRenderPasses.
    /// @note Must be called after all AddRenderPass calls are done.
    auto Setup() -> Result<void>;
    auto Compile() -> void;
    auto Execute(VkCommandBuffer t_cmd_buf) -> void;
    auto Clear() -> void;

    [[nodiscard]] auto GetResourceNode(RenderGraphResourceHandle t_handle) -> render_graph_resource_node&;
    [[nodiscard]] auto GetCompiledRenderPass(std::size_t t_pass_index) -> compiled_render_pass&;

    [[nodiscard]] auto
    ImportTexture(std::string_view t_name, VkImage t_image, VkImageView t_view, VkFormat t_format, VkExtent)
        -> RenderGraphResourceHandle;
private:
    auto acquire_resource_slot_index() -> std::size_t;
    auto create_handle(RendererResourceType t_type) -> RenderGraphResourceHandle;
    auto destroy_handle(RenderGraphResourceHandle t_handle) -> void;
    auto is_handle_valid(RenderGraphResourceHandle t_handle) -> bool;

    VkDevice m_device{VK_NULL_HANDLE};

    std::vector<std::unique_ptr<IRenderPass>> m_passes;

    std::vector<render_graph_resource_node> m_resources;
    std::vector<std::uint16_t>              m_handle_versions;
    std::vector<std::size_t>                m_free_resource_indices;

    std::vector<compiled_render_pass> m_compiled_passes;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_GRAPH_HPP_
