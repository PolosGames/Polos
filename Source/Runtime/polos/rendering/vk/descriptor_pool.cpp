///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/descriptor_pool.hpp"

#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

#include <algorithm>
#include <utility>

namespace polos::rendering
{

DescriptorPool::DescriptorPool(
    std::span<VkDescriptorPoolSize const> tPoolSizes,
    uint32_t const                        tMaxSets,
    VkDescriptorPoolCreateFlags const     tFlags)
    : mDevice{VulkanDevice::Get().logiDevice}
{
    VkDescriptorPoolCreateInfo const poolInfo{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = tFlags,
        .maxSets       = tMaxSets,
        .poolSizeCount = ToVkSize(tPoolSizes.size()),
        .pPoolSizes    = tPoolSizes.data(),
    };

    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mPool), "Failed to create descriptor pool");
}

DescriptorPool::DescriptorPool(DescriptorPool&& tOther)
    : mDevice{std::exchange(tOther.mDevice, VK_NULL_HANDLE)},
      mPool{std::exchange(tOther.mPool, VK_NULL_HANDLE)},
      mSetGroups{std::move(tOther.mSetGroups)}
{}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& tOther)
{
    if (this != &tOther)
    {
        reset();

        mDevice    = std::exchange(tOther.mDevice, VK_NULL_HANDLE);
        mPool      = std::exchange(tOther.mPool, VK_NULL_HANDLE);
        mSetGroups = std::move(tOther.mSetGroups);
    }

    return *this;
}

DescriptorPool::~DescriptorPool()
{
    reset();
}

void DescriptorPool::reset()
{
    if (VK_NULL_HANDLE == mDevice)
    {
        return;
    }

    vkDestroyDescriptorPool(mDevice, std::exchange(mPool, VK_NULL_HANDLE), nullptr);

    for (auto& group : mSetGroups)
    {
        for (auto const& set : group.sets)
        {
            set->mHandle = VK_NULL_HANDLE;
        }

        vkDestroyDescriptorSetLayout(mDevice, group.layout, nullptr);
    }

    mSetGroups.clear();
    mDevice = VK_NULL_HANDLE;
}

void DescriptorPool::createSet(
    std::span<VkDescriptorSetLayoutBinding const> const tBindings,
    std::uint32_t const                                 tSetCount,
    VkDescriptorSetLayoutCreateFlags const              tLayoutFlags,
    void const* const                                   tLayoutNext,
    void const* const                                   tAllocationNext)
{
    VkDescriptorSetLayoutCreateInfo const layoutInfo{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = tLayoutNext,
        .flags        = tLayoutFlags,
        .bindingCount = ToVkSize(tBindings.size()),
        .pBindings    = tBindings.data(),
    };

    auto& group = mSetGroups.emplace_back();
    group.bindings.reserve(tBindings.size());
    for (auto const& binding : tBindings)
    {
        group.bindings.push_back(
            Binding{
                .index = binding.binding,
                .type  = binding.descriptorType,
            });
    }

    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorSetLayout(mDevice, &layoutInfo, nullptr, &group.layout),
        "Failed to create descriptor set layout");

    std::vector<VkDescriptorSetLayout> const layouts(tSetCount, group.layout);
    std::vector<VkDescriptorSet>             handles(tSetCount);

    VkDescriptorSetAllocateInfo const allocateInfo{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = tAllocationNext,
        .descriptorPool     = mPool,
        .descriptorSetCount = tSetCount,
        .pSetLayouts        = layouts.data(),
    };

    PL_ASSERT(
        VK_SUCCESS == vkAllocateDescriptorSets(mDevice, &allocateInfo, handles.data()),
        "Failed to allocate descriptor sets");

    group.sets.reserve(tSetCount);
    for (auto* handle : handles)
    {
        group.sets.push_back(std::shared_ptr<DescriptorSet>{new DescriptorSet{handle, group.bindings.size()}});
    }
}

auto DescriptorPool::GetSet(std::uint32_t const tSetIndex, std::uint32_t const tFrameSlot) const
    -> std::weak_ptr<DescriptorSet>
{
    return mSetGroups[tSetIndex].sets[tFrameSlot];
}

auto DescriptorPool::GetSetHandle(std::uint32_t const tSetIndex, std::uint32_t const tFrameSlot) const
    -> VkDescriptorSet
{
    return mSetGroups[tSetIndex].sets[tFrameSlot]->mHandle;
}

auto DescriptorPool::GetSetLayoutAtIndex(std::uint32_t const tSetIndex) const -> VkDescriptorSetLayout
{
    return mSetGroups[tSetIndex].layout;
}

void DescriptorPool::UpdateBuffers(
    std::uint32_t const                        tSetIndex,
    std::uint32_t const                        tFrameSlot,
    std::initializer_list<BufferBinding> const tBindings) const
{
    auto const&                       group          = mSetGroups[tSetIndex];
    auto const&                       set            = group.sets[tFrameSlot];
    auto const&                       layoutBindings = group.bindings;
    std::vector<VkWriteDescriptorSet> writes;

    for (auto const& input : tBindings)
    {
        auto const binding = std::ranges::find(layoutBindings, input.binding, &Binding::index);

        auto& bound = set->mBufferBindings[static_cast<std::size_t>(binding - layoutBindings.begin())];
        if (bound.buffer == input.buffer && bound.offset == input.offset && bound.range == input.range)
        {
            continue;
        }

        if (writes.empty())
        {
            writes.reserve(tBindings.size());
        }

        bound = {
            .buffer = input.buffer,
            .offset = input.offset,
            .range  = input.range,
        };

        writes.push_back({
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
            .dstSet           = set->mHandle,
            .dstBinding       = input.binding,
            .dstArrayElement  = 0U,
            .descriptorCount  = 1U,
            .descriptorType   = binding->type,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &bound,
            .pTexelBufferView = nullptr,
        });
    }

    if (!writes.empty())
    {
        vkUpdateDescriptorSets(mDevice, ToVkSize(writes.size()), writes.data(), 0U, nullptr);
    }
}

void DescriptorPool::UpdateImage(
    std::uint32_t const          tSetIndex,
    std::uint32_t const          tBinding,
    std::uint32_t const          tArrayElement,
    VkDescriptorImageInfo const& tImage,
    std::uint32_t const          tFrameSlot) const
{
    auto* const set = GetSetHandle(tSetIndex, tFrameSlot);

    auto const&                layoutBindings = mSetGroups[tSetIndex].bindings;
    auto const                 binding        = std::ranges::find(layoutBindings, tBinding, &Binding::index);
    VkWriteDescriptorSet const write{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = set,
        .dstBinding       = tBinding,
        .dstArrayElement  = tArrayElement,
        .descriptorCount  = 1U,
        .descriptorType   = binding->type,
        .pImageInfo       = &tImage,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(mDevice, 1U, &write, 0U, nullptr);
}

}// namespace polos::rendering
