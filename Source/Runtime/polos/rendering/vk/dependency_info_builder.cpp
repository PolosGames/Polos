///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/dependency_info_builder.hpp"

#include "polos/rendering/vk/vulkan_utils.hpp"

#include <utility>

namespace polos::rendering
{

void DependencyInfo::UpdateBuffer(std::size_t const tBarrierIndex, VkBuffer tBuffer)
{
    auto& buffer = mBufferBarriers[tBarrierIndex].buffer;
    if (buffer != tBuffer)
    {
        buffer = tBuffer;
    }
}

auto DependencyInfo::GetInfo() const& -> VkDependencyInfo
{
    return {
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                    = nullptr,
        .dependencyFlags          = mFlags,
        .memoryBarrierCount       = ToVkSize(mMemoryBarriers.size()),
        .pMemoryBarriers          = mMemoryBarriers.data(),
        .bufferMemoryBarrierCount = ToVkSize(mBufferBarriers.size()),
        .pBufferMemoryBarriers    = mBufferBarriers.data(),
        .imageMemoryBarrierCount  = ToVkSize(mImageBarriers.size()),
        .pImageMemoryBarriers     = mImageBarriers.data(),
    };
}

auto DependencyInfoBuilder::SetFlags(VkDependencyFlags const tFlags) -> DependencyInfoBuilder&
{
    mInfo.mFlags = tFlags;
    return *this;
}

auto DependencyInfoBuilder::AddMemoryBarrier(Scope const tSource, Scope const tDestination) -> DependencyInfoBuilder&
{
    mInfo.mMemoryBarriers.push_back({
        .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .pNext         = nullptr,
        .srcStageMask  = tSource.stages,
        .srcAccessMask = tSource.access,
        .dstStageMask  = tDestination.stages,
        .dstAccessMask = tDestination.access,
    });
    return *this;
}

auto DependencyInfoBuilder::AddBufferBarrier(
    VkBuffer           tBuffer,
    Scope const        tSource,
    Scope const        tDestination,
    VkDeviceSize const tOffset,
    VkDeviceSize const tSize) -> DependencyInfoBuilder&
{
    mInfo.mBufferBarriers.push_back({
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .pNext               = nullptr,
        .srcStageMask        = tSource.stages,
        .srcAccessMask       = tSource.access,
        .dstStageMask        = tDestination.stages,
        .dstAccessMask       = tDestination.access,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer              = tBuffer,
        .offset              = tOffset,
        .size                = tSize,
    });
    return *this;
}

auto DependencyInfoBuilder::AddImageBarrier(
    ImageTransition const& tImage,
    Scope const            tSource,
    Scope const            tDestination) -> DependencyInfoBuilder&
{
    mInfo.mImageBarriers.push_back({
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext               = nullptr,
        .srcStageMask        = tSource.stages,
        .srcAccessMask       = tSource.access,
        .dstStageMask        = tDestination.stages,
        .dstAccessMask       = tDestination.access,
        .oldLayout           = tImage.oldLayout,
        .newLayout           = tImage.newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = tImage.image,
        .subresourceRange    = tImage.range,
    });
    return *this;
}

auto DependencyInfoBuilder::Finalize() const& -> DependencyInfo
{
    return mInfo;
}

auto DependencyInfoBuilder::Finalize() && -> DependencyInfo
{
    return std::move(mInfo);
}

}// namespace polos::rendering
