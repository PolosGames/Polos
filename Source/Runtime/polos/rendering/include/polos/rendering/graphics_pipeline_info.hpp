///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP
#define POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP

#include "polos/rendering/shader.hpp"
#include "polos/rendering/vertex.hpp"
#include "polos/utils/string_id.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>
#include <span>

namespace polos::rendering
{

struct graphics_pipeline_info
{
    utils::string_id name;

    std::span<shader const*> shaders;

    VkPrimitiveTopology                   topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    std::optional<VertexInputDescription> vertex_input;

    VkPolygonMode   polygon_mode{VK_POLYGON_MODE_FILL};
    VkCullModeFlags cull_mode{VK_CULL_MODE_BACK_BIT};
    VkFrontFace     front_face{VK_FRONT_FACE_CLOCKWISE};
    VkBool32        depth_bias_enable{VK_FALSE};

    VkSampleCountFlagBits multisampling{VK_SAMPLE_COUNT_1_BIT};

    VkBool32    depth_test_enable{VK_TRUE};
    VkBool32    depth_write_enable{VK_TRUE};
    VkCompareOp depth_compare_op{VK_COMPARE_OP_LESS};

    std::span<VkPipelineColorBlendAttachmentState const> color_blend_attachments;

    std::span<VkDynamicState const> dynamic_states;

    VkRenderPass  render_pass{VK_NULL_HANDLE};
    std::uint32_t subpass{0U};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_GRAPHICS_PIPELINE_INFO_HPP
