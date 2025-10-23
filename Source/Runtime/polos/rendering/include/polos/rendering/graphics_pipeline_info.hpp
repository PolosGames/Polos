//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/shader.hpp"
#include "polos/rendering/vertex.hpp"

#include <cstdint>
#include <string>

namespace polos::rendering
{

struct graphics_pipeline_info
{
    std::string name;

    shader vertex_shader;
    shader fragment_shader;

    VkPrimitiveTopology    topology;
    VertexInputDescription vertex_input;

    VkPolygonMode   polygon_mode;
    VkCullModeFlags cull_mode;
    VkFrontFace     front_face;
    bool            depth_bias_enable;

    VkSampleCountFlagBits multisampling;

    bool        depth_test_enable;
    bool        depth_write_enable;
    VkCompareOp depth_compare_op;

    // For a shadow pass, this will be empty as we don't write to color targets.
    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments;

    std::vector<VkDynamicState> dynamic_states;

    VkRenderPass  render_pass{VK_NULL_HANDLE};
    std::uint32_t subpass{0U};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP_
