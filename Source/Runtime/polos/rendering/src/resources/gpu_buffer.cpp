///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "resources/gpu_buffer.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "vk/common.hpp"

namespace polos::rendering
{

VmaAllocator GpuBuffer::sAllocator{VK_NULL_HANDLE};

GpuBuffer::GpuBuffer(GpuBuffer&& tOther)
{ swap(tOther); }

GpuBuffer::~GpuBuffer()
{
    if (nullptr != mapping)
    {
        vmaUnmapMemory(sAllocator, allocation);
    }
    if (VK_NULL_HANDLE != buffer)
    {
        vmaDestroyBuffer(sAllocator, buffer, allocation);
    }
}

auto GpuBuffer::operator=(GpuBuffer&& tOther) -> GpuBuffer&
{
    if (this == &tOther)
    {
        return *this;
    }

    destroy();
    swap(tOther);

    return *this;
}

auto GpuBuffer::Create(BufferDescription const& tDesc) -> Result<std::unique_ptr<GpuBuffer>>
{
    VkBufferCreateInfo const bufferInfo{
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .size                  = tDesc.size,
        .usage                 = tDesc.usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0U,
        .pQueueFamilyIndices   = nullptr,
    };

    VmaAllocationCreateInfo const allocInfo{
        .flags = static_cast<VmaAllocationCreateFlags>(tDesc.hostAccess),
        .usage = static_cast<VmaMemoryUsage>(tDesc.residence),
        .requiredFlags =
            (0U != (tDesc.hostAccess & HostAccessFlags::kSeqWrite)) ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : 0U,
        .preferredFlags = 0U,
        .memoryTypeBits = 0U,
        .pool           = VK_NULL_HANDLE,
        .pUserData      = nullptr,
        .priority       = 1.0F,
    };

    auto buffer = new GpuBuffer();

    CHECK_VK_SUCCESS_OR_ERR(
        vmaCreateBuffer(sAllocator, &bufferInfo, &allocInfo, &buffer->buffer, &buffer->allocation, nullptr),
        RenderingErrc::kFailedCreateBuffer);

    if (0U != (tDesc.hostAccess & HostAccessFlags::kSeqWrite))
    {
        vmaMapMemory(sAllocator, buffer->allocation, &buffer->mapping);
    }

    return std::unique_ptr<GpuBuffer>(buffer);
}

auto GpuBuffer::destroy() -> void
{
    if (nullptr != mapping)
    {
        vmaUnmapMemory(sAllocator, allocation);
    }
    if (VK_NULL_HANDLE != buffer)
    {
        LogInfo("Destroying VkBuffer while moving the resource. Is this intended?");
        vmaDestroyBuffer(sAllocator, buffer, allocation);
    }

    allocation = VK_NULL_HANDLE;
    buffer     = VK_NULL_HANDLE;
    size       = 0U;
    mapping    = nullptr;
}

auto GpuBuffer::swap(GpuBuffer& tOther) -> void
{
    std::swap(allocation, tOther.allocation);
    std::swap(buffer, tOther.buffer);
    std::swap(residence, tOther.residence);
    std::swap(hostAccess, tOther.hostAccess);
    std::swap(size, tOther.size);
    std::swap(mapping, tOther.mapping);
}

}// namespace polos::rendering