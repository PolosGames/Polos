///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/descriptor_pool.hpp"

#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{

DescriptorPool::DescriptorPool(std::span<VkDescriptorPoolSize> tPoolSizes, uint32_t const tMaxSets)
{
    VkDescriptorPoolCreateInfo dpci{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0U,
        .maxSets       = tMaxSets,
        .poolSizeCount = ToVkSize(tPoolSizes.size()),
        .pPoolSizes    = tPoolSizes.data(),
    };

    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorPool(RenderContext::GetVulkanDevice().logiDevice, &dpci, nullptr, &pool),
        "Failed to create DescriptorPool for ForwardShadingPass");
}

DescriptorPool::DescriptorPool(DescriptorPool&& tOther)
    : pool(std::exchange(tOther.pool, VK_NULL_HANDLE)),
      descriptorSets(std::move(tOther.descriptorSets))
{}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& tOther)
{
    if (this == &tOther)
    {
        return *this;
    }

    if (VK_NULL_HANDLE != pool)
    {
        vkDestroyDescriptorPool(RenderContext::GetVulkanDevice().logiDevice, pool, nullptr);
    }

    pool           = std::exchange(tOther.pool, VK_NULL_HANDLE);
    descriptorSets = std::move(tOther.descriptorSets);

    return *this;
}

DescriptorPool::~DescriptorPool()
{
    if (nullptr != pool)
    {
        vkDestroyDescriptorPool(RenderContext::GetVulkanDevice().logiDevice, pool, nullptr);
    }
}

auto DescriptorPool::CreateSet(std::shared_ptr<DescriptorSetLayout>& tLayout) -> uint32_t
{
    auto index = static_cast<uint32_t>(descriptorSets.size());
    descriptorSets.push_back({.first = DescriptorSet::Create(pool, tLayout->layout), .second = tLayout});

    return index;
}

auto DescriptorPool::GetSetForFrameSlotAtIndex(uint32_t const tSetIndex, uint32_t const tFrameSlot) -> DescriptorSet&
{
    return descriptorSets[tSetIndex].first[tFrameSlot];
}

auto DescriptorPool::GetSetLayoutAtIndex(uint32_t tSetIndex) -> VkDescriptorSetLayout
{
    return descriptorSets[tSetIndex].second->layout;
}

}// namespace polos::rendering
