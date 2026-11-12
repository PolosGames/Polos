///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/descriptor_pool_builder.hpp"

#include <algorithm>

namespace polos::rendering
{

void DescriptorPoolBuilder::SetFlags(VkDescriptorPoolCreateFlags const tFlags)
{
    mFlags = tFlags;
}

void DescriptorPoolBuilder::AddPoolSize(VkDescriptorType const tType, std::uint32_t const tCount)
{
    auto const size = std::ranges::find(mPoolSizes, tType, &VkDescriptorPoolSize::type);
    if (size != mPoolSizes.end())
    {
        size->descriptorCount += tCount;
        return;
    }

    mPoolSizes.push_back({.type = tType, .descriptorCount = tCount});
}

auto DescriptorPoolBuilder::SetDefinition::AddBinding(VkDescriptorSetLayoutBinding const tBinding) -> SetDefinition&
{
    mBindings.push_back(tBinding);
    return *this;
}

auto DescriptorPoolBuilder::SetDefinition::SetLayoutFlags(VkDescriptorSetLayoutCreateFlags const tFlags)
    -> SetDefinition&
{
    mLayoutFlags = tFlags;
    return *this;
}

auto DescriptorPoolBuilder::SetDefinition::SetLayoutNext(void const* const tNext) -> SetDefinition&
{
    mLayoutNext = tNext;
    return *this;
}

auto DescriptorPoolBuilder::SetDefinition::SetAllocationNext(void const* const tNext) -> SetDefinition&
{
    mAllocationNext = tNext;
    return *this;
}

auto DescriptorPoolBuilder::AddSet(std::uint32_t const tCount) -> SetDefinition&
{
    auto& set  = mSets.emplace_back();
    set.mCount = tCount;
    return set;
}

auto DescriptorPoolBuilder::Finalize() const -> DescriptorPool
{
    std::uint32_t maxSets{0U};
    for (auto const& set : mSets)
    {
        maxSets += set.mCount;
    }

    DescriptorPool pool{mPoolSizes, maxSets, mFlags};
    for (auto const& set : mSets)
    {
        pool.createSet(set.mBindings, set.mCount, set.mLayoutFlags, set.mLayoutNext, set.mAllocationNext);
    }

    return pool;
}

}// namespace polos::rendering
