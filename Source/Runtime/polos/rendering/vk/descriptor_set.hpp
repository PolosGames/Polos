///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_DESCRIPTOR_SET_HPP
#define POLOS_RENDERING_VK_DESCRIPTOR_SET_HPP

#include <vulkan/vulkan.h>

#include <cstddef>
#include <vector>

namespace polos::rendering
{

class DescriptorSet
{
    friend class DescriptorPool;

    DescriptorSet(VkDescriptorSet tHandle, std::size_t tBindingCount)
        : mHandle{tHandle},
          mBufferBindings(tBindingCount)
    {}

    VkDescriptorSet                     mHandle{VK_NULL_HANDLE};
    std::vector<VkDescriptorBufferInfo> mBufferBindings;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VK_DESCRIPTOR_SET_HPP
