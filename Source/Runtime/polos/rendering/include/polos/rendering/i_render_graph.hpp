//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_GRAPH_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_GRAPH_HPP_

#include "polos/rendering/common.hpp"

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

    virtual Result<void> Create(render_graph_creation_details const& t_device) = 0;
    virtual Result<void> Destroy()                                             = 0;

    template<typename PassType, typename... Args>
        requires std::is_base_of_v<IRenderPass, PassType>
    auto AddRenderPass(Args&&... args) -> IRenderPass*
    {
        m_passes.emplace_back(new PassType(std::forward<Args>(args)...));

        return (*m_passes.rbegin()).get();
    }

    virtual void Compile()                      = 0;
    virtual void Execute(VkCommandBuffer t_buf) = 0;
    virtual void Reset()                        = 0;
protected:
    std::vector<std::unique_ptr<IRenderPass>> m_passes;
    IRenderContext*                           m_context;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_GRAPH_HPP_
