///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_RENDER_PASS_BUILDER_HPP
#define POLOS_RENDERING_VK_RENDER_PASS_BUILDER_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/vk/vulkan_render_pass.hpp"

#include <cstdint>
#include <deque>
#include <optional>
#include <vector>

namespace polos::rendering
{

struct AttachmentDescription
{
    VkFormat              format{VK_FORMAT_UNDEFINED};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
    VkAttachmentLoadOp    loadOp{VK_ATTACHMENT_LOAD_OP_DONT_CARE};
    VkAttachmentStoreOp   storeOp{VK_ATTACHMENT_STORE_OP_DONT_CARE};
    VkAttachmentLoadOp    stencilLoadOp{VK_ATTACHMENT_LOAD_OP_DONT_CARE};
    VkAttachmentStoreOp   stencilStoreOp{VK_ATTACHMENT_STORE_OP_DONT_CARE};
    VkImageLayout         initialLayout{VK_IMAGE_LAYOUT_UNDEFINED};
    VkImageLayout         finalLayout{VK_IMAGE_LAYOUT_UNDEFINED};
};

struct AttachmentReference
{
    std::uint32_t      attachment{VK_ATTACHMENT_UNUSED};
    VkImageLayout      layout{VK_IMAGE_LAYOUT_UNDEFINED};
    VkImageAspectFlags aspectMask{0U};
};

struct SubpassDependency
{
    std::uint32_t         srcSubpass{VK_SUBPASS_EXTERNAL};
    std::uint32_t         dstSubpass{VK_SUBPASS_EXTERNAL};
    VkPipelineStageFlags2 srcStageMask{0U};
    VkAccessFlags2        srcAccessMask{0U};
    VkPipelineStageFlags2 dstStageMask{0U};
    VkAccessFlags2        dstAccessMask{0U};
    VkDependencyFlags     dependencyFlags{0U};
};

class POLOS_API RenderPassBuilder
{
public:
    class POLOS_API SubpassDefinition
    {
    public:
        auto AddInputReference(AttachmentReference tReference) -> SubpassDefinition&;
        auto AddColorReference(AttachmentReference tReference) -> SubpassDefinition&;
        auto AddDepthReference(AttachmentReference tReference) -> SubpassDefinition&;
        auto AddDependency(SubpassDependency tDependency) -> SubpassDefinition&;
    private:
        friend class RenderPassBuilder;

        std::vector<VkAttachmentReference2>   mInputReferences;
        std::vector<VkAttachmentReference2>   mColorReferences;
        std::optional<VkAttachmentReference2> mDepthReference;
        std::vector<SubpassDependency>        mDependencies;
    };

    auto AddAttachment(AttachmentDescription tAttachment) -> RenderPassBuilder&;
    auto AddSubpass() -> SubpassDefinition&;

    [[nodiscard]] auto Finalize() const -> VulkanRenderPass;
private:
    std::vector<VkAttachmentDescription2> mAttachments;
    std::deque<SubpassDefinition>         mSubpasses;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VK_RENDER_PASS_BUILDER_HPP
