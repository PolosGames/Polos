//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_CLEAR_SCREEN_PASS_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_CLEAR_SCREEN_PASS_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/i_render_pass.hpp"

namespace polos::rendering
{

struct render_graph_resource_node;

class ClearScreenPass : public IRenderPass
{
public:
    ClearScreenPass(RenderGraphResourceHandle t_handle);
    ~ClearScreenPass() override = default;

    void        Setup(RenderPassResolver& t_builder) override;
    void        Execute(VkCommandBuffer t_cmd_buf, const RenderGraphRegistry& t_registry) override;
    char const* Name() const override;
private:
    RenderGraphResourceHandle m_sc_img_handle;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_CLEAR_SCREEN_PASS_HPP_
