///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_I_RENDER_GRAPH_HPP
#define POLOS_RENDERING_I_RENDER_GRAPH_HPP

#include "polos/communication/error_code.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace polos::rendering
{

class IRenderContext;
class IRenderPass;


struct render_graph_creation_details
{
    VkDevice        device;
    IRenderContext* context;
};

class IRenderGraph
{
public:
    virtual ~IRenderGraph() = default;

    virtual auto Create(render_graph_creation_details const& t_device) -> Result<void> = 0;
    virtual auto Destroy() -> Result<void>                                             = 0;

    template<typename PassType, typename... Args>
        requires std::is_base_of_v<IRenderPass, PassType>
    auto AddRenderPass(Args&&... args) -> IRenderPass*
    {
        m_passes.emplace_back(new PassType(std::forward<Args>(args)...));

        return (*m_passes.rbegin()).get();
    }

    virtual auto Compile() -> void                      = 0;
    virtual auto Execute(VkCommandBuffer t_buf) -> void = 0;
    virtual auto Reset() -> void                        = 0;
protected:
    std::vector<std::unique_ptr<IRenderPass>> m_passes;
    IRenderContext*                           m_context{nullptr};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_I_RENDER_GRAPH_HPP
