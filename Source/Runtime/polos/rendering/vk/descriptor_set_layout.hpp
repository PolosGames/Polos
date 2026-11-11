///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_DESCRIPTOR_SET_LAYOUT_HPP
#define POLOS_RENDERING_VK_DESCRIPTOR_SET_LAYOUT_HPP

#include <vulkan/vulkan.h>

#include <memory>
#include <span>

namespace polos::rendering
{
struct DescriptorSetLayout
{
    VkDescriptorSetLayout layout;

    [[nodiscard]] static auto Create(std::span<VkDescriptorSetLayoutBinding const> tBindings)
        -> std::shared_ptr<DescriptorSetLayout>;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_VK_DESCRIPTOR_SET_LAYOUT_HPP
