///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/ssbo/quad_instance.hpp"

#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{

auto CreateQuadInstanceStorageBuffer(std::size_t tInstanceCount) -> std::unique_ptr<Buffer>
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(RenderContext::GetVulkanDevice().physDevice, &properties);
    PL_ASSERT(
        tInstanceCount <= properties.limits.maxStorageBufferRange / sizeof(QuadInstance),
        "Quad instance data for {} quads exceeds the device's storage buffer range.",
        tInstanceCount);

    auto buffer = Buffer::Create(
        BufferDescription{
            .size       = sizeof(QuadInstance) * tInstanceCount,
            .usage      = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            .residence  = MemoryResidence::kHost,
            .hostAccess = HostAccessFlags::kSeqWrite,
        });

    PL_ASSERT(nullptr != buffer, "Could not create instance buffers");

    return buffer;
}

}// namespace polos::rendering
