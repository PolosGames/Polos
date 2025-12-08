///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_PASS_CLEAR_SCREEN_PASS_HPP
#define POLOS_RENDERING_RENDER_PASS_CLEAR_SCREEN_PASS_HPP

#include "polos/rendering/common.hpp"
#include "polos/rendering/i_render_pass.hpp"

namespace polos::rendering
{

struct render_graph_resource_node;

class ClearScreenPass : public IRenderPass
{
public:
    explicit ClearScreenPass(RenderGraphResourceHandle t_handle);
    ~ClearScreenPass() override = default;

    auto               Setup(RenderPassResolver& t_builder) -> void override;
    auto               Execute(VkCommandBuffer t_cmd_buf, const RenderGraphRegistry& t_registry) -> void override;
    [[nodiscard]] auto Name() const -> char const* override;
private:
    RenderGraphResourceHandle m_sc_img_handle;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_PASS_CLEAR_SCREEN_PASS_HPP
