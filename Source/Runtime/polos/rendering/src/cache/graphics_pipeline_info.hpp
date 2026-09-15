///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_CACHE_GRAPHICS_PIPELINE_INFO_HPP
#define POLOS_RENDERING_SRC_CACHE_GRAPHICS_PIPELINE_INFO_HPP

#include "resources/shader.hpp"
#include "resources/vertex.hpp"
#include "polos/utils/string_id.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>
#include <span>

namespace polos::rendering
{

struct GraphicsPipelineInfo
{
    utils::string_id name;

    std::span<Shader const*> shaders;

    VkPrimitiveTopology                   topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    std::optional<VertexInputDescription> vertexInput;

    VkPolygonMode   polygonMode{VK_POLYGON_MODE_FILL};
    VkCullModeFlags cullMode{VK_CULL_MODE_BACK_BIT};
    VkFrontFace     frontFace{VK_FRONT_FACE_CLOCKWISE};
    VkBool32        depthBiasEnable{VK_FALSE};

    VkSampleCountFlagBits multisampling{VK_SAMPLE_COUNT_1_BIT};

    VkBool32    depthTestEnable{VK_TRUE};
    VkBool32    depthWriteEnable{VK_TRUE};
    VkCompareOp depthCompareOp{VK_COMPARE_OP_LESS};

    std::span<VkPipelineColorBlendAttachmentState const> colorBlendAttachments;

    std::span<VkDynamicState const> dynamicStates;

    VkRenderPass  renderPass{VK_NULL_HANDLE};
    std::uint32_t subpass{0U};

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBinding;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_CACHE_GRAPHICS_PIPELINE_INFO_HPP
