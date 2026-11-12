///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/command_buffer.hpp"

#include "polos/rendering/vk/dependency_info_builder.hpp"
#include "polos/rendering/vk/vulkan_render_pass.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"

namespace polos::rendering
{

CommandBuffer::CommandBuffer(VkCommandBuffer tHandle)
    : mHandle{tHandle}
{}

CommandBuffer::operator bool() const
{
    return mHandle != VK_NULL_HANDLE;
}

auto CommandBuffer::GetHandle() const -> VkCommandBuffer
{
    return mHandle;
}

void CommandBuffer::BeginRenderPass(
    VulkanRenderPass const&             tRenderPass,
    VkFramebuffer                       tFramebuffer,
    VkRect2D const&                     tRenderArea,
    std::span<VkClearValue const> const tClearValues,
    VkSubpassContents const             tContents)
{
    tRenderPass.Begin(*this, tFramebuffer, tRenderArea, tClearValues, tContents);
}

void CommandBuffer::beginRenderPass(VkRenderPassBeginInfo const& tInfo, VkSubpassContents const tContents) const
{
    vkCmdBeginRenderPass(mHandle, &tInfo, tContents);
}

void CommandBuffer::EndRenderPass() const
{
    vkCmdEndRenderPass(mHandle);
}

void CommandBuffer::SetScissor(VkRect2D const& tScissor) const
{
    vkCmdSetScissor(mHandle, 0U, 1U, &tScissor);
}

void CommandBuffer::SetViewport(VkViewport const& tViewport) const
{
    vkCmdSetViewport(mHandle, 0U, 1U, &tViewport);
}

void CommandBuffer::BindPipeline(VkPipelineBindPoint const tBindPoint, VkPipeline tPipeline) const
{
    vkCmdBindPipeline(mHandle, tBindPoint, tPipeline);
}

void CommandBuffer::BindDescriptorSets(
    VkPipelineBindPoint const              tBindPoint,
    VkPipelineLayout                       tLayout,
    std::uint32_t const                    tFirstSet,
    std::span<VkDescriptorSet const> const tSets) const
{
    vkCmdBindDescriptorSets(mHandle, tBindPoint, tLayout, tFirstSet, ToVkSize(tSets.size()), tSets.data(), 0U, nullptr);
}

void CommandBuffer::BindVertexBuffer(VkBuffer tBuffer, VkDeviceSize const tOffset, std::uint32_t const tBinding) const
{
    vkCmdBindVertexBuffers(mHandle, tBinding, 1U, &tBuffer, &tOffset);
}

void CommandBuffer::BindIndexBuffer(VkBuffer tBuffer, VkDeviceSize const tOffset, VkIndexType const tIndexType) const
{
    vkCmdBindIndexBuffer(mHandle, tBuffer, tOffset, tIndexType);
}

void CommandBuffer::PushConstants(
    VkPipelineLayout                 tLayout,
    VkShaderStageFlags const         tStages,
    std::uint32_t const              tOffset,
    std::span<std::byte const> const tData) const
{
    vkCmdPushConstants(mHandle, tLayout, tStages, tOffset, ToVkSize(tData.size()), tData.data());
}

void CommandBuffer::Dispatch(
    std::uint32_t const tGroupCountX,
    std::uint32_t const tGroupCountY,
    std::uint32_t const tGroupCountZ) const
{
    vkCmdDispatch(mHandle, tGroupCountX, tGroupCountY, tGroupCountZ);
}

void CommandBuffer::DrawIndexedIndirect(
    VkBuffer            tBuffer,
    VkDeviceSize const  tOffset,
    std::uint32_t const tDrawCount,
    std::uint32_t const tStride) const
{
    vkCmdDrawIndexedIndirect(mHandle, tBuffer, tOffset, tDrawCount, tStride);
}

void CommandBuffer::PipelineBarrier(
    VkPipelineStageFlags const                   tSrcStages,
    VkPipelineStageFlags const                   tDstStages,
    std::span<VkBufferMemoryBarrier const> const tBufferBarriers,
    std::span<VkImageMemoryBarrier const> const  tImageBarriers) const
{
    vkCmdPipelineBarrier(
        mHandle, tSrcStages, tDstStages, 0U, 0U, nullptr, ToVkSize(tBufferBarriers.size()), tBufferBarriers.data(),
        ToVkSize(tImageBarriers.size()), tImageBarriers.data());
}

void CommandBuffer::PipelineBarrier2(DependencyInfo const& tInfo) const
{
    VkDependencyInfo const& info = tInfo.GetInfo();
    vkCmdPipelineBarrier2(mHandle, &info);
}

void CommandBuffer::CopyBuffer(VkBuffer tSource, VkBuffer tDestination, VkBufferCopy const& tRegion) const
{
    vkCmdCopyBuffer(mHandle, tSource, tDestination, 1U, &tRegion);
}

void CommandBuffer::CopyBufferToImage(
    VkBuffer                 tSource,
    VkImage                  tDestination,
    VkImageLayout const      tLayout,
    VkBufferImageCopy const& tRegion) const
{
    vkCmdCopyBufferToImage(mHandle, tSource, tDestination, tLayout, 1U, &tRegion);
}

void CommandBuffer::BlitImage2(VkBlitImageInfo2 const& tInfo) const
{
    vkCmdBlitImage2(mHandle, &tInfo);
}

}// namespace polos::rendering
