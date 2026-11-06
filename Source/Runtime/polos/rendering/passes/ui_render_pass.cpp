///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/passes/ui_render_pass.hpp"

#include "polos/gui/backend.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_util.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{
namespace
{
auto CreateRenderPass(VkDevice tDevice, VkFormat tColorFormat) -> VkRenderPass
{
    VkAttachmentDescription const color{
        .flags          = 0U,
        .format         = tColorFormat,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .finalLayout    = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    };
    VkAttachmentReference const colorReference{0U, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkSubpassDescription const  subpass{
        .flags                   = 0U,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0U,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1U,
        .pColorAttachments       = &colorReference,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0U,
        .pPreserveAttachments    = nullptr,
    };
    std::array<VkSubpassDependency, 2U> const dependencies{{
        {
            .srcSubpass      = VK_SUBPASS_EXTERNAL,
            .dstSubpass      = 0U,
            .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = 0U,
        },
        {
            .srcSubpass      = 0U,
            .dstSubpass      = VK_SUBPASS_EXTERNAL,
            .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_TRANSFER_BIT,
            .srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask   = VK_ACCESS_TRANSFER_READ_BIT,
            .dependencyFlags = 0U,
        },
    }};
    VkRenderPassCreateInfo const              passInfo{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .attachmentCount = 1U,
        .pAttachments    = &color,
        .subpassCount    = 1U,
        .pSubpasses      = &subpass,
        .dependencyCount = 2U,
        .pDependencies   = dependencies.data(),
    };
    VkRenderPass pass{VK_NULL_HANDLE};
    if (vkCreateRenderPass(tDevice, &passInfo, nullptr, &pass) != VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }
    return pass;
}
}// namespace

auto UiRenderPass::Create(VkFormat tColorFormat) -> bool
{
    auto const device = RenderContext::GetVulkanDevice().logiDevice;
    auto const pass   = CreateRenderPass(device, tColorFormat);
    if (pass == VK_NULL_HANDLE)
    {
        return false;
    }

    if (!gui::backend::Initialize(pass))
    {
        vkDestroyRenderPass(device, pass, nullptr);
        return false;
    }

    mRenderPass = pass;
    return true;
}

UiRenderPass::~UiRenderPass()
{
    if (mRenderPass != VK_NULL_HANDLE)
    {
        gui::backend::Shutdown();
        ReleaseFramebuffers();
        vkDestroyRenderPass(RenderContext::GetVulkanDevice().logiDevice, mRenderPass, nullptr);
    }
}

auto UiRenderPass::ReleaseFramebuffers() -> void
{
    auto const device = RenderContext::GetVulkanDevice().logiDevice;
    for (auto& framebuffer : mFramebuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}

auto UiRenderPass::Record(Inputs const& tInputs) -> void
{
    auto& framebuffer = mFramebuffers[tInputs.frameSlot];
    if (framebuffer == VK_NULL_HANDLE)
    {
        std::array attachments{tInputs.colorView};
        framebuffer = util::CreateFramebuffer(
            RenderContext::GetVulkanDevice().logiDevice,
            mRenderPass,
            attachments,
            tInputs.extent);
        PL_ASSERT(framebuffer != VK_NULL_HANDLE, "Could not create the GUI framebuffer.");
    }

    VkRenderPassBeginInfo const begin{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext           = nullptr,
        .renderPass      = mRenderPass,
        .framebuffer     = framebuffer,
        .renderArea      = {.offset = {0, 0}, .extent = tInputs.extent},
        .clearValueCount = 0U,
        .pClearValues    = nullptr,
    };
    vkCmdBeginRenderPass(tInputs.cmdBuf, &begin, VK_SUBPASS_CONTENTS_INLINE);
    gui::backend::RecordDrawData(tInputs.cmdBuf);
    vkCmdEndRenderPass(tInputs.cmdBuf);
}

}// namespace polos::rendering
