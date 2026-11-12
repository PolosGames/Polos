///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_COMMAND_BUFFER_HPP
#define POLOS_RENDERING_VK_COMMAND_BUFFER_HPP

#include "polos/polos_api.hpp"

#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstdint>
#include <span>

namespace polos::rendering
{

class DependencyInfo;
class VulkanRenderPass;

class POLOS_API CommandBuffer
{
public:
    CommandBuffer() = default;
    explicit CommandBuffer(VkCommandBuffer tHandle);

    explicit operator bool() const;

    [[nodiscard]] auto GetHandle() const -> VkCommandBuffer;

    void BeginRenderPass(
        VulkanRenderPass const&       tRenderPass,
        VkFramebuffer                 tFramebuffer,
        VkRect2D const&               tRenderArea,
        std::span<VkClearValue const> tClearValues = {},
        VkSubpassContents             tContents    = VK_SUBPASS_CONTENTS_INLINE);
    void EndRenderPass() const;
    void SetScissor(VkRect2D const& tScissor) const;
    void SetViewport(VkViewport const& tViewport) const;

    void BindPipeline(VkPipelineBindPoint tBindPoint, VkPipeline tPipeline) const;
    void BindDescriptorSets(
        VkPipelineBindPoint              tBindPoint,
        VkPipelineLayout                 tLayout,
        std::uint32_t                    tFirstSet,
        std::span<VkDescriptorSet const> tSets) const;
    void BindVertexBuffer(VkBuffer tBuffer, VkDeviceSize tOffset, std::uint32_t tBinding = 0U) const;
    void BindIndexBuffer(VkBuffer tBuffer, VkDeviceSize tOffset, VkIndexType tIndexType) const;

    void PushConstants(
        VkPipelineLayout           tLayout,
        VkShaderStageFlags         tStages,
        std::uint32_t              tOffset,
        std::span<std::byte const> tData) const;
    void Dispatch(std::uint32_t tGroupCountX, std::uint32_t tGroupCountY, std::uint32_t tGroupCountZ) const;
    void
    DrawIndexedIndirect(VkBuffer tBuffer, VkDeviceSize tOffset, std::uint32_t tDrawCount, std::uint32_t tStride) const;

    void PipelineBarrier(
        VkPipelineStageFlags                   tSrcStages,
        VkPipelineStageFlags                   tDstStages,
        std::span<VkBufferMemoryBarrier const> tBufferBarriers,
        std::span<VkImageMemoryBarrier const>  tImageBarriers = {}) const;
    void PipelineBarrier2(DependencyInfo const& tInfo) const;
    void CopyBuffer(VkBuffer tSource, VkBuffer tDestination, VkBufferCopy const& tRegion) const;
    void CopyBufferToImage(
        VkBuffer                 tSource,
        VkImage                  tDestination,
        VkImageLayout            tLayout,
        VkBufferImageCopy const& tRegion) const;
    void BlitImage2(VkBlitImageInfo2 const& tInfo) const;
private:
    friend class VulkanRenderPass;

    void beginRenderPass(VkRenderPassBeginInfo const& tInfo, VkSubpassContents tContents) const;

    VkCommandBuffer mHandle{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif
