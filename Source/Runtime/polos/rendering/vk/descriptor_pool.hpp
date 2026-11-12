///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_DESCRIPTOR_POOL_HPP
#define POLOS_RENDERING_VK_DESCRIPTOR_POOL_HPP

#include "polos/rendering/vk/descriptor_set.hpp"
#include "polos/rendering/vk/descriptor_set_layout.hpp"
#include "polos/utils/pair.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace polos::rendering
{
class DescriptorPool
{
public:
    using SetEntry =
        utils::Pair<std::array<DescriptorSet, Settings::kMaxFramesInFlight>, std::shared_ptr<DescriptorSetLayout>>;
    using Pool = VkDescriptorPool;

    DescriptorPool() = default;
    explicit DescriptorPool(
        std::span<VkDescriptorPoolSize const> tPoolSizes,
        uint32_t                              tMaxSets,
        VkDescriptorPoolCreateFlags           tFlags = 0U);
    DescriptorPool(DescriptorPool const&) = delete;
    DescriptorPool(DescriptorPool&& tOther);

    DescriptorPool& operator=(DescriptorPool const&) = delete;
    DescriptorPool& operator=(DescriptorPool&& tOther);
    ~DescriptorPool();

    [[nodiscard]] auto AllocateSet(VkDescriptorSetLayout tLayout, void const* tNext = nullptr) const -> DescriptorSet;
    auto               CreateSet(std::shared_ptr<DescriptorSetLayout>& tLayout) -> uint32_t;
    auto               GetSetForFrameSlotAtIndex(uint32_t tSetIndex, uint32_t tFrameSlot) -> DescriptorSet&;
    auto               GetSetLayoutAtIndex(uint32_t tSetIndex) -> VkDescriptorSetLayout;

    Pool                  pool{VK_NULL_HANDLE};
    std::vector<SetEntry> descriptorSets;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_VK_DESCRIPTOR_POOL_HPP
