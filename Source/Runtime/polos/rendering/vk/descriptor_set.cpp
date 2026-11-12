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

auto DescriptorSet::Create(VkDescriptorPool tPool, VkDescriptorSetLayout tLayout)
    -> std::array<DescriptorSet, Settings::kMaxFramesInFlight>
{
    std::array<VkDescriptorSetLayout, Settings::kMaxFramesInFlight> layouts;
    layouts.fill(tLayout);

    VkDescriptorSetAllocateInfo const allocateInfo{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = tPool,
        .descriptorSetCount = ToVkSize(layouts.size()),
        .pSetLayouts        = layouts.data(),
    };

    std::array<VkDescriptorSet, Settings::kMaxFramesInFlight> handles;
    PL_ASSERT(
        VK_SUCCESS ==
            vkAllocateDescriptorSets(VulkanDevice::Get().logiDevice, &allocateInfo, handles.data()),
        "Failed to allocate descriptor sets");

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

    vkUpdateDescriptorSets(
        VulkanDevice::Get().logiDevice,
        ToVkSize(tWrites.size()),
        tWrites.data(),
        0U,
        nullptr);
}

}// namespace polos::rendering
