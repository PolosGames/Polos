///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/render_pass_builder.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"

namespace polos::rendering
{

namespace
{

auto ToVulkanReference(AttachmentReference const& tReference) -> VkAttachmentReference2
{
    return {
        .sType      = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
        .pNext      = nullptr,
        .attachment = tReference.attachment,
        .layout     = tReference.layout,
        .aspectMask = tReference.aspectMask,
    };
}

}// namespace

auto RenderPassBuilder::SubpassDefinition::AddInputReference(AttachmentReference const tReference) -> SubpassDefinition&
{
    mInputReferences.push_back(ToVulkanReference(tReference));
    return *this;
}

auto RenderPassBuilder::SubpassDefinition::AddColorReference(AttachmentReference const tReference) -> SubpassDefinition&
{
    mColorReferences.push_back(ToVulkanReference(tReference));
    return *this;
}

auto RenderPassBuilder::SubpassDefinition::AddDepthReference(AttachmentReference const tReference) -> SubpassDefinition&
{
    mDepthReference = ToVulkanReference(tReference);
    return *this;
}

auto RenderPassBuilder::SubpassDefinition::AddDependency(SubpassDependency const tDependency) -> SubpassDefinition&
{
    mDependencies.push_back(tDependency);
    return *this;
}

auto RenderPassBuilder::AddAttachment(AttachmentDescription const tAttachment) -> RenderPassBuilder&
{
    mAttachments.push_back({
        .sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
        .pNext          = nullptr,
        .flags          = 0U,
        .format         = tAttachment.format,
        .samples        = tAttachment.samples,
        .loadOp         = tAttachment.loadOp,
        .storeOp        = tAttachment.storeOp,
        .stencilLoadOp  = tAttachment.stencilLoadOp,
        .stencilStoreOp = tAttachment.stencilStoreOp,
        .initialLayout  = tAttachment.initialLayout,
        .finalLayout    = tAttachment.finalLayout,
    });
    return *this;
}

auto RenderPassBuilder::AddSubpass() -> SubpassDefinition&
{
    return mSubpasses.emplace_back();
}

auto RenderPassBuilder::Finalize() const -> VulkanRenderPass
{
    std::deque<VkMemoryBarrier2>       barriers;
    std::vector<VkSubpassDescription2> subpasses;
    std::vector<VkSubpassDependency2>  dependencies;
    subpasses.reserve(mSubpasses.size());

    for (auto const& subpass : mSubpasses)
    {
        subpasses.push_back({
            .sType                   = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
            .pNext                   = nullptr,
            .flags                   = 0U,
            .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .viewMask                = 0U,
            .inputAttachmentCount    = ToVkSize(subpass.mInputReferences.size()),
            .pInputAttachments       = subpass.mInputReferences.data(),
            .colorAttachmentCount    = ToVkSize(subpass.mColorReferences.size()),
            .pColorAttachments       = subpass.mColorReferences.data(),
            .pResolveAttachments     = nullptr,
            .pDepthStencilAttachment = subpass.mDepthReference ? &*subpass.mDepthReference : nullptr,
            .preserveAttachmentCount = 0U,
            .pPreserveAttachments    = nullptr,
        });

        for (auto const& dependency : subpass.mDependencies)
        {
            auto& barrier = barriers.emplace_back(
                VkMemoryBarrier2{
                    .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                    .pNext         = nullptr,
                    .srcStageMask  = dependency.srcStageMask,
                    .srcAccessMask = dependency.srcAccessMask,
                    .dstStageMask  = dependency.dstStageMask,
                    .dstAccessMask = dependency.dstAccessMask,
                });

            dependencies.push_back({
                .sType           = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
                .pNext           = &barrier,
                .srcSubpass      = dependency.srcSubpass,
                .dstSubpass      = dependency.dstSubpass,
                .srcStageMask    = 0U,
                .dstStageMask    = 0U,
                .srcAccessMask   = 0U,
                .dstAccessMask   = 0U,
                .dependencyFlags = dependency.dependencyFlags,
                .viewOffset      = 0,
            });
        }
    }

    VkRenderPassCreateInfo2 const createInfo{
        .sType                   = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .pNext                   = nullptr,
        .flags                   = 0U,
        .attachmentCount         = ToVkSize(mAttachments.size()),
        .pAttachments            = mAttachments.data(),
        .subpassCount            = ToVkSize(subpasses.size()),
        .pSubpasses              = subpasses.data(),
        .dependencyCount         = ToVkSize(dependencies.size()),
        .pDependencies           = dependencies.data(),
        .correlatedViewMaskCount = 0U,
        .pCorrelatedViewMasks    = nullptr,
    };

    VkDevice     device = VulkanDevice::Get().logiDevice;
    VkRenderPass handle{VK_NULL_HANDLE};

    if (vkCreateRenderPass2(device, &createInfo, nullptr, &handle) != VK_SUCCESS)
    {
        LogError("Failed to create render pass.");
        return {};
    }

    return {device, handle};
}

}// namespace polos::rendering
