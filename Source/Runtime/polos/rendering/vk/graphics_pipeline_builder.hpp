///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_GRAPHICS_PIPELINE_BUILDER_HPP
#define POLOS_RENDERING_GRAPHICS_PIPELINE_BUILDER_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/vertex/vertex_input_description.hpp"
#include "polos/rendering/vk/vulkan_pipeline.hpp"
#include "polos/utils/string_id.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <vector>

namespace polos::rendering
{

struct Shader;

class POLOS_API GraphicsPipelineBuilder
{
public:
    GraphicsPipelineBuilder()                               = default;
    GraphicsPipelineBuilder(GraphicsPipelineBuilder const&) = delete;
    GraphicsPipelineBuilder(GraphicsPipelineBuilder&&)      = default;
    ~GraphicsPipelineBuilder()                              = default;

    auto operator=(GraphicsPipelineBuilder const&) -> GraphicsPipelineBuilder& = delete;
    auto operator=(GraphicsPipelineBuilder&&) -> GraphicsPipelineBuilder&      = default;

    void SetName(utils::string_id tName);
    void AddShader(std::shared_ptr<Shader const> tShader);
    void SetVertexInput(VertexInputDescription tInput);
    void SetTopology(VkPrimitiveTopology tTopology);
    void SetRasterization(
        VkPolygonMode   tPolygonMode,
        VkCullModeFlags tCullMode,
        VkFrontFace     tFrontFace,
        VkBool32        tDepthBiasEnable = VK_FALSE);
    void SetMultisampling(VkSampleCountFlagBits tSamples);
    void SetDepthState(VkBool32 tTest, VkBool32 tWrite, VkCompareOp tCompareOp);
    void AddColorAttachment(VkPipelineColorBlendAttachmentState tAttachment);
    void SetDynamicStates(std::span<VkDynamicState const> tStates);
    void SetViewport(VkViewport tViewport, VkRect2D tScissor);
    void SetRenderPass(VkRenderPass tRenderPass, std::uint32_t tSubpass = 0U);
    void AddDescriptorSetLayout(VkDescriptorSetLayout tLayout);
    void AddPushConstantRange(VkPushConstantRange tRange);

    [[nodiscard]] auto Finalize() const -> VulkanPipeline;
private:
    utils::string_id mName{0};

    std::vector<std::shared_ptr<Shader const>> mShaders;

    VkPrimitiveTopology                   mTopology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    std::optional<VertexInputDescription> mVertexInput;

    VkPolygonMode   mPolygonMode{VK_POLYGON_MODE_FILL};
    VkCullModeFlags mCullMode{VK_CULL_MODE_BACK_BIT};
    VkFrontFace     mFrontFace{VK_FRONT_FACE_CLOCKWISE};
    VkBool32        mDepthBiasEnable{VK_FALSE};

    VkSampleCountFlagBits mMultisampling{VK_SAMPLE_COUNT_1_BIT};

    VkBool32    mDepthTestEnable{VK_TRUE};
    VkBool32    mDepthWriteEnable{VK_TRUE};
    VkCompareOp mDepthCompareOp{VK_COMPARE_OP_LESS};

    std::vector<VkPipelineColorBlendAttachmentState> mColorBlendAttachments;

    std::vector<VkDynamicState> mDynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkViewport mViewport{.x = 0.0F, .y = 0.0F, .width = 1.0F, .height = 1.0F, .minDepth = 0.0F, .maxDepth = 1.0F};
    VkRect2D   mScissor{.offset = {.x = 0, .y = 0}, .extent = {.width = 1U, .height = 1U}};

    VkRenderPass  mRenderPass{VK_NULL_HANDLE};
    std::uint32_t mSubpass{0U};

    std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
    std::vector<VkPushConstantRange>   mPushConstantRanges;
};

}

#endif
