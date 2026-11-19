///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_DESCRIPTOR_BINDING_HPP
#define POLOS_RENDERING_VK_DESCRIPTOR_BINDING_HPP

#include <vulkan/vulkan.h>

#include <cstdint>

namespace polos::rendering
{
struct DescriptorBinding
{
    VkDescriptorType type{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER};
    uint32_t         set{0U};
    uint32_t         binding{0U};
    uint32_t         countPerFrame{1U};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VK_DESCRIPTOR_BINDING_HPP
