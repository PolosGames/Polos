///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_DESCRIPTOR_POOL_BUILDER_HPP
#define POLOS_RENDERING_VK_DESCRIPTOR_POOL_BUILDER_HPP

#include "polos/rendering/vk/descriptor_pool.hpp"

#include <cstdint>
#include <deque>
#include <vector>

namespace polos::rendering
{

class DescriptorPoolBuilder
{
public:
    class SetDefinition
    {
    public:
        auto AddBinding(VkDescriptorSetLayoutBinding tBinding) -> SetDefinition&;
        auto SetLayoutFlags(VkDescriptorSetLayoutCreateFlags tFlags) -> SetDefinition&;
        auto SetLayoutNext(void const* tNext) -> SetDefinition&;
        auto SetAllocationNext(void const* tNext) -> SetDefinition&;
    private:
        friend class DescriptorPoolBuilder;

        std::vector<VkDescriptorSetLayoutBinding> mBindings;
        std::uint32_t                             mCount{1U};
        VkDescriptorSetLayoutCreateFlags          mLayoutFlags{0U};
        void const*                               mLayoutNext{nullptr};
        void const*                               mAllocationNext{nullptr};
    };

    void SetFlags(VkDescriptorPoolCreateFlags tFlags);
    void AddPoolSize(VkDescriptorType tType, std::uint32_t tCount);

    auto AddSet(std::uint32_t tCount = 1U) -> SetDefinition&;

    [[nodiscard]] auto Finalize() const -> DescriptorPool;
private:
    VkDescriptorPoolCreateFlags       mFlags{0U};
    std::vector<VkDescriptorPoolSize> mPoolSizes;
    std::deque<SetDefinition>         mSets;
};

}

#endif
