///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/descriptor_set.hpp"

#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

#include <cstddef>

namespace polos::rendering
{

namespace
{

auto AllocateSets(
    VkDescriptorPool                       tPool,
    std::span<VkDescriptorSetLayout const> tLayouts,
    VkDescriptorSet*                       tHandles,
    void const*                            tNext) -> void
{
    VkDescriptorSetAllocateInfo const allocateInfo{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = tNext,
        .descriptorPool     = tPool,
        .descriptorSetCount = ToVkSize(tLayouts.size()),
        .pSetLayouts        = tLayouts.data(),
    };

    PL_ASSERT(
        VK_SUCCESS == vkAllocateDescriptorSets(VulkanDevice::Get().logiDevice, &allocateInfo, tHandles),
        "Failed to allocate descriptor sets");
}

}// namespace

auto DescriptorSet::CreateSingle(VkDescriptorPool tPool, VkDescriptorSetLayout tLayout, void const* tNext)
    -> DescriptorSet
{
    DescriptorSet descriptorSet;
    AllocateSets(tPool, std::span{&tLayout, 1U}, &descriptorSet.set, tNext);
    return descriptorSet;
}

auto DescriptorSet::Create(VkDescriptorPool tPool, VkDescriptorSetLayout tLayout)
    -> std::array<DescriptorSet, Settings::kMaxFramesInFlight>
{
    std::array<VkDescriptorSetLayout, Settings::kMaxFramesInFlight> layouts;
    layouts.fill(tLayout);

    std::array<VkDescriptorSet, Settings::kMaxFramesInFlight> handles;
    AllocateSets(tPool, layouts, handles.data(), nullptr);

    std::array<DescriptorSet, Settings::kMaxFramesInFlight> sets;
    for (std::size_t slot{0U}; slot < sets.size(); ++slot)
    {
        sets[slot].set = handles[slot];
    }

    return sets;
}

auto DescriptorSet::Update(std::span<VkWriteDescriptorSet> tWrites) const -> void
{
    for (auto& write : tWrites)
    {
        write.dstSet = set;
    }

    vkUpdateDescriptorSets(VulkanDevice::Get().logiDevice, ToVkSize(tWrites.size()), tWrites.data(), 0U, nullptr);
}

}// namespace polos::rendering
