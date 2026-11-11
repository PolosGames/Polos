///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/vk/descriptor_set_layout.hpp"

#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{

auto DescriptorSetLayout::Create(std::span<VkDescriptorSetLayoutBinding const> tBindings)
    -> std::shared_ptr<DescriptorSetLayout>
{
    VkDescriptorSetLayoutCreateInfo dslci{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = 0U,
        .bindingCount = ToVkSize(tBindings.size()),
        .pBindings    = tBindings.data(),
    };

    VkDescriptorSetLayout dsl{VK_NULL_HANDLE};
    PL_ASSERT(
        VK_SUCCESS == vkCreateDescriptorSetLayout(RenderContext::GetVulkanDevice().logiDevice, &dslci, nullptr, &dsl),
        "Failed to create Descriptor set layout for ForwardShadingPass");

    return {new DescriptorSetLayout{.layout = dsl}, [](DescriptorSetLayout* tPtr) {
                vkDestroyDescriptorSetLayout(RenderContext::GetVulkanDevice().logiDevice, tPtr->layout, nullptr);
                delete tPtr;
            }};
}

}// namespace polos::rendering
