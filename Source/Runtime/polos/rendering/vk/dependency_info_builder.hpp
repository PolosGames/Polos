///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_DEPENDENCY_INFO_BUILDER_HPP
#define POLOS_RENDERING_VK_DEPENDENCY_INFO_BUILDER_HPP

#include <vulkan/vulkan.h>

#include <cstddef>
#include <vector>

namespace polos::rendering
{

class DependencyInfoBuilder;

class DependencyInfo
{
public:
    void UpdateBuffer(std::size_t tBarrierIndex, VkBuffer tBuffer);

    [[nodiscard]] auto GetInfo() const& -> VkDependencyInfo;
    [[nodiscard]] auto GetInfo() const&& -> VkDependencyInfo = delete;
private:
    friend class DependencyInfoBuilder;

    VkDependencyFlags                   mFlags{};
    std::vector<VkMemoryBarrier2>       mMemoryBarriers;
    std::vector<VkBufferMemoryBarrier2> mBufferBarriers;
    std::vector<VkImageMemoryBarrier2>  mImageBarriers;
};

class DependencyInfoBuilder
{
public:
    struct Scope
    {
        VkPipelineStageFlags2 stages;
        VkAccessFlags2        access;
    };

    struct ImageTransition
    {
        VkImage                 image;
        VkImageLayout           oldLayout;
        VkImageLayout           newLayout;
        VkImageSubresourceRange range;
    };

    auto SetFlags(VkDependencyFlags tFlags) -> DependencyInfoBuilder&;
    auto AddMemoryBarrier(Scope tSource, Scope tDestination) -> DependencyInfoBuilder&;
    auto AddBufferBarrier(
        VkBuffer     tBuffer,
        Scope        tSource,
        Scope        tDestination,
        VkDeviceSize tOffset = 0U,
        VkDeviceSize tSize   = VK_WHOLE_SIZE) -> DependencyInfoBuilder&;
    auto AddImageBarrier(ImageTransition const& tImage, Scope tSource, Scope tDestination) -> DependencyInfoBuilder&;

    [[nodiscard]] auto Finalize() const& -> DependencyInfo;
    [[nodiscard]] auto Finalize() && -> DependencyInfo;
private:
    DependencyInfo mInfo;
};

}// namespace polos::rendering

#endif
