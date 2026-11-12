///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/descriptor_set_layout.hpp"

#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{

auto DescriptorSetLayout::Create(
    std::span<VkDescriptorSetLayoutBinding const> tBindings,
    VkDescriptorSetLayoutCreateFlags tFlags,
    void const* tNext)
    -> std::shared_ptr<DescriptorSetLayout>
{
    VkDescriptorSetLayoutCreateInfo const layoutInfo{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = tNext,
        .flags        = tFlags,
        .bindingCount = ToVkSize(tBindings.size()),
        .pBindings    = tBindings.data(),
    };

    VkDescriptorSetLayout dsl{VK_NULL_HANDLE};
    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorSetLayout(VulkanDevice::Get().logiDevice, &layoutInfo, nullptr, &dsl),
        "Failed to create descriptor set layout");

    return {new DescriptorSetLayout{.layout = dsl}, [](DescriptorSetLayout* tPtr) {
                vkDestroyDescriptorSetLayout(VulkanDevice::Get().logiDevice, tPtr->layout, nullptr);
                delete tPtr;
            }};
}

}// namespace polos::rendering
