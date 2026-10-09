///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/resources/buffer.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk/render_context.hpp"

namespace polos::rendering
{

VmaAllocator Buffer::sAllocator{VK_NULL_HANDLE};

Buffer::Buffer(Buffer&& tOther)
{
    swap(tOther);
}

Buffer::~Buffer()
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

auto Buffer::operator=(Buffer&& tOther) -> Buffer&
{
    if (this == &tOther)
    {
        return *this;
    }

    destroy();
    swap(tOther);

    return *this;
}

auto Buffer::Create(BufferDescription const& tDesc) -> std::unique_ptr<Buffer>
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
        .flags          = static_cast<VmaAllocationCreateFlags>(tDesc.hostAccess),
        .usage          = static_cast<VmaMemoryUsage>(tDesc.residence),
        .requiredFlags  = tDesc.hostAccess & HostAccessFlags::kSeqWrite ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : 0U,
        .preferredFlags = 0U,
        .memoryTypeBits = 0U,
        .pool           = VK_NULL_HANDLE,
        .pUserData      = nullptr,
        .priority       = 1.0F,
    };

    auto buffer = std::unique_ptr<Buffer>(new Buffer());

    if (VK_SUCCESS !=
        vmaCreateBuffer(sAllocator, &bufferInfo, &allocInfo, &buffer->buffer, &buffer->allocation, nullptr))
    {
        LogError("Failed to create VMA buffer allocation.");
        return nullptr;
    }

    buffer->size       = tDesc.size;
    buffer->residence  = tDesc.residence;
    buffer->hostAccess = tDesc.hostAccess;

    if (tDesc.persistentlyMapped && tDesc.hostAccess != HostAccessFlags::kNone)
    {
        if (VK_SUCCESS != vmaMapMemory(sAllocator, buffer->allocation, &buffer->mapping))
        {
            LogError("Failed to map VMA buffer allocation.");
            return nullptr;
        }
    }

    return buffer;
}

auto Buffer::Create(BufferDescription const& tDesc, std::span<std::byte const> tInitialData) -> std::unique_ptr<Buffer>
{
    if (tInitialData.empty() || tInitialData.size_bytes() != tDesc.size)
    {
        LogError("Initial buffer data must be nonempty and match the buffer size.");
        return nullptr;
    }

    auto destinationDesc = tDesc;
    destinationDesc.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    destinationDesc.persistentlyMapped = false;
    auto destination                   = Create(destinationDesc);
    if (!destination)
    {
        return nullptr;
    }

    auto staging = Create(
        BufferDescription{
            .size               = tDesc.size,
            .usage              = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .residence          = MemoryResidence::kHost,
            .hostAccess         = HostAccessFlags::kSeqWrite,
            .persistentlyMapped = false,
        });

    if (!staging || !staging->Write(tInitialData.data(), tInitialData.size_bytes()))
    {
        LogError("Failed to upload initial buffer data to staging memory.");
        return nullptr;
    }

    VkCommandBuffer cmdBuf = RenderContext::BeginSingleTimeCommands();
    if (cmdBuf == VK_NULL_HANDLE)
    {
        return nullptr;
    }

    VkBufferCopy const copy{
        .srcOffset = 0U,
        .dstOffset = 0U,
        .size      = tDesc.size,
    };
    vkCmdCopyBuffer(cmdBuf, staging->buffer, destination->buffer, 1U, &copy);

    VkBufferMemoryBarrier const barrier{
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer              = destination->buffer,
        .offset              = 0U,
        .size                = tDesc.size,
    };
    vkCmdPipelineBarrier(
        cmdBuf,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0U,
        0U,
        nullptr,
        1U,
        &barrier,
        0U,
        nullptr);

    if (!RenderContext::EndSingleTimeCommands(cmdBuf))
    {
        return nullptr;
    }

    return destination;
}

auto Buffer::SetAllocator(VmaAllocator tAllocator) -> void
{
    sAllocator = tAllocator;
}

auto Buffer::destroy() -> void
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

auto Buffer::swap(Buffer& tOther) -> void
{
    std::swap(allocation, tOther.allocation);
    std::swap(buffer, tOther.buffer);
    std::swap(residence, tOther.residence);
    std::swap(hostAccess, tOther.hostAccess);
    std::swap(size, tOther.size);
    std::swap(mapping, tOther.mapping);
}

}// namespace polos::rendering
