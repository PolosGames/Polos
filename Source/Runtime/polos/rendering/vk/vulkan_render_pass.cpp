///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/vulkan_render_pass.hpp"

#include "polos/rendering/vk/command_buffer.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"

#include <utility>

namespace polos::rendering
{

VulkanRenderPass::VulkanRenderPass(VkDevice tDevice, VkRenderPass tHandle)
    : mDevice{tDevice},
      mHandle{tHandle}
{}

VulkanRenderPass::VulkanRenderPass(VulkanRenderPass&& tOther) noexcept
    : mDevice{std::exchange(tOther.mDevice, VK_NULL_HANDLE)},
      mHandle{std::exchange(tOther.mHandle, VK_NULL_HANDLE)}
{}

auto VulkanRenderPass::operator=(VulkanRenderPass&& tOther) noexcept -> VulkanRenderPass&
{
    if (this != &tOther)
    {
        VulkanRenderPass previous{std::move(*this)};
        mDevice = std::exchange(tOther.mDevice, VK_NULL_HANDLE);
        mHandle = std::exchange(tOther.mHandle, VK_NULL_HANDLE);
    }

    return *this;
}

VulkanRenderPass::~VulkanRenderPass()
{
    if (mHandle != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(mDevice, mHandle, nullptr);
    }
}

auto VulkanRenderPass::GetHandle() const -> VkRenderPass
{
    return mHandle;
}

void VulkanRenderPass::Begin(
    CommandBuffer& tCmdBuf,
    VkFramebuffer                       tFramebuffer,
    VkRect2D const&                     tRenderArea,
    std::span<VkClearValue const> const tClearValues,
    VkSubpassContents const             tContents) const
{
    VkRenderPassBeginInfo const beginInfo{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext           = nullptr,
        .renderPass      = mHandle,
        .framebuffer     = tFramebuffer,
        .renderArea      = tRenderArea,
        .clearValueCount = ToVkSize(tClearValues.size()),
        .pClearValues    = tClearValues.data(),
    };

    tCmdBuf.beginRenderPass(beginInfo, tContents);
}

}// namespace polos::rendering
