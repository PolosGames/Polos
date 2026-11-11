///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_DESCRIPTOR_SET_HPP
#define POLOS_RENDERING_VK_DESCRIPTOR_SET_HPP

#include "polos/rendering/settings.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <span>

namespace polos::rendering
{

// The Vulkan descriptor pool owns the handles.
class DescriptorSet
{
public:
    [[nodiscard]] static auto Create(VkDescriptorPool tPool, VkDescriptorSetLayout tLayout)
        -> std::array<DescriptorSet, Settings::kMaxFramesInFlight>;

    // Sets dstSet on each write before submitting the batch.
    auto Update(std::span<VkWriteDescriptorSet> tWrites) const -> void;

    VkDescriptorSet set{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VK_DESCRIPTOR_SET_HPP
