///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/passes/ui_render_pass.hpp"

#include "polos/gui/backend.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/vk/command_buffer.hpp"
#include "polos/rendering/vk/render_pass_builder.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

#include <utility>

namespace polos::rendering
{
namespace
{
auto CreateRenderPass(VkFormat const tColorFormat) -> VulkanRenderPass
{
    RenderPassBuilder builder;
    builder.AddAttachment({
        .format        = tColorFormat,
        .samples       = kSceneSampleCount,
        .loadOp        = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp       = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .finalLayout   = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    });

    builder.AddSubpass()
        .AddColorReference({
            .attachment = 0U,
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        })
        .AddDependency({
            .srcSubpass    = VK_SUBPASS_EXTERNAL,
            .dstSubpass    = 0U,
            .srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        })
        .AddDependency({
            .srcSubpass    = 0U,
            .dstSubpass    = VK_SUBPASS_EXTERNAL,
            .srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
        });

    return builder.Finalize();
}
}// namespace

auto UiRenderPass::Create(VkFormat tColorFormat) -> bool
{
    auto pass = CreateRenderPass(tColorFormat);
    if (pass.GetHandle() == VK_NULL_HANDLE)
    {
        return false;
    }

    if (!gui::backend::Initialize(pass.GetHandle()))
    {
        return false;
    }

    mRenderPass = std::move(pass);
    return true;
}

UiRenderPass::~UiRenderPass()
{
    if (mRenderPass.GetHandle() != VK_NULL_HANDLE)
    {
        gui::backend::Shutdown();
        ReleaseFramebuffers();
    }
}

auto UiRenderPass::ReleaseFramebuffers() -> void
{
    auto* const device = VulkanDevice::Get().logiDevice;
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
        framebuffer = util::CreateFramebuffer(mRenderPass.GetHandle(), attachments, tInputs.extent);
        PL_ASSERT(framebuffer != VK_NULL_HANDLE, "Could not create the GUI framebuffer.");
    }

    auto& cmdBuf = tInputs.cmdBuf;
    cmdBuf.BeginRenderPass(mRenderPass, framebuffer, {.offset = {0, 0}, .extent = tInputs.extent});
    gui::backend::RecordDrawData(cmdBuf);
    cmdBuf.EndRenderPass();
}

}// namespace polos::rendering
