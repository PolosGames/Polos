///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/compute_pipeline_builder.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/resources/shader.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"

#include <utility>

namespace polos::rendering
{

void ComputePipelineBuilder::SetShader(std::shared_ptr<Shader const> tShader)
{
    mShader = std::move(tShader);
}

void ComputePipelineBuilder::SetFlags(VkPipelineCreateFlags const tFlags)
{
    mFlags = tFlags;
}

void ComputePipelineBuilder::AddDescriptorSetLayout(VkDescriptorSetLayout tLayout)
{
    mLayouts.push_back(tLayout);
}

void ComputePipelineBuilder::AddPushConstantRange(VkPushConstantRange const tRange)
{
    mPushConstants.push_back(tRange);
}

void ComputePipelineBuilder::SetSpecializationConstants(
    std::span<VkSpecializationMapEntry const> const tEntries,
    std::span<std::byte const> const                tData)
{
    mSpecializationEntries.assign(tEntries.begin(), tEntries.end());
    mSpecializationData.assign(tData.begin(), tData.end());
}

auto ComputePipelineBuilder::Finalize() const -> VulkanPipeline
{
    VkSpecializationInfo const specialization{
        .mapEntryCount = ToVkSize(mSpecializationEntries.size()),
        .pMapEntries   = mSpecializationEntries.data(),
        .dataSize      = mSpecializationData.size(),
        .pData         = mSpecializationData.data(),
    };

    if (mShader == nullptr)
    {
        LogError("Missing compute shader.");
        return {};
    }

    VkPipelineShaderStageCreateInfo shaderStage{
        .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0U,
        .stage               = VK_SHADER_STAGE_COMPUTE_BIT,
        .module              = mShader->module,
        .pName               = "main",
        .pSpecializationInfo = mSpecializationEntries.empty() ? nullptr : &specialization,
    };

    VkPipelineLayoutCreateInfo layoutInfo{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0U,
        .setLayoutCount         = ToVkSize(mLayouts.size()),
        .pSetLayouts            = mLayouts.data(),
        .pushConstantRangeCount = ToVkSize(mPushConstants.size()),
        .pPushConstantRanges    = mPushConstants.data(),
    };

    VkDevice       device = VulkanDevice::Get().logiDevice;
    VulkanPipeline result;

    if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &result.layout) != VK_SUCCESS)
    {
        LogError("Failed to create compute pipeline layout.");
        return {};
    }

    VkComputePipelineCreateInfo pipelineInfo{
        .sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = mFlags,
        .stage              = shaderStage,
        .layout             = result.layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex  = 0U,
    };

    if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1U, &pipelineInfo, nullptr, &result.pipeline) != VK_SUCCESS)
    {
        LogError("Failed to create compute pipeline.");
        VulkanPipeline::Reset(result);
        return {};
    }

    return result;
}

}// namespace polos::rendering
