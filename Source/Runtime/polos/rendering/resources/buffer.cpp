///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/resources/buffer.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/utils/assert.hpp"

#include <algorithm>
#include <utility>

namespace polos::rendering
{

namespace
{

auto CopyBuffer(
    VkBuffer     tSource,
    VkBuffer     tDestination,
    VkDeviceSize tSize,
    VkDeviceSize tSrcOffset = 0U,
    VkDeviceSize tDstOffset = 0U) -> bool
{
    VkCommandBuffer cmdBuf = RenderContext::Get().BeginSingleTimeCommands();
    if (cmdBuf == VK_NULL_HANDLE)
    {
        return false;
    }

    VkBufferMemoryBarrier const sourceBarrier{
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = VK_ACCESS_MEMORY_WRITE_BIT,
        .dstAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer              = tSource,
        .offset              = tSrcOffset,
        .size                = tSize,
    };
    vkCmdPipelineBarrier(
        cmdBuf,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0U,
        0U,
        nullptr,
        1U,
        &sourceBarrier,
        0U,
        nullptr);

    VkBufferCopy const copy{
        .srcOffset = tSrcOffset,
        .dstOffset = tDstOffset,
        .size      = tSize,
    };
    vkCmdCopyBuffer(cmdBuf, tSource, tDestination, 1U, &copy);

    VkBufferMemoryBarrier const barrier{
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_HOST_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer              = tDestination,
        .offset              = tDstOffset,
        .size                = tSize,
    };
    vkCmdPipelineBarrier(
        cmdBuf,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT | VK_PIPELINE_STAGE_HOST_BIT,
        0U,
        0U,
        nullptr,
        1U,
        &barrier,
        0U,
        nullptr);

    return RenderContext::Get().EndSingleTimeCommands(cmdBuf);
}

}// namespace

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
    auto usage = tDesc.usage;
    if (tDesc.resizable)
    {
        usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    VkBufferCreateInfo const bufferInfo{
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0U,
        .size                  = tDesc.size,
        .usage                 = usage,
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

    buffer->mUsage     = tDesc.usage;
    buffer->mResizable = tDesc.resizable;
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
            .resizable          = false,
        });

    if (!staging || !staging->Write(tInitialData.data(), tInitialData.size_bytes()))
    {
        LogError("Failed to upload initial buffer data to staging memory.");
        return nullptr;
    }

    if (!CopyBuffer(staging->buffer, destination->buffer, tDesc.size))
    {
        return nullptr;
    }

    return destination;
}

auto Buffer::Resize(VkDeviceSize tSize) -> bool
{
    if (!mResizable || tSize == 0U || buffer == VK_NULL_HANDLE)
    {
        return false;
    }
    if (tSize == size)
    {
        return true;
    }

    auto replacement = Create(
        BufferDescription{
            .size               = tSize,
            .usage              = mUsage,
            .residence          = residence,
            .hostAccess         = hostAccess,
            .persistentlyMapped = mapping != nullptr,
            .resizable          = mResizable,
        });

    PL_ASSERT(nullptr != replacement, "Replacement buffer could not be created when resizing!");
    PL_ASSERT(
        CopyBuffer(buffer, replacement->buffer, std::min(size, tSize)),
        "Buffer could not be copied into the replacement buffer!");

    swap(*replacement);
    return true;
}

auto Buffer::Upload(std::span<std::byte const> tData, VkDeviceSize const tDstOffset) -> bool
{
    if (tData.empty() || buffer == VK_NULL_HANDLE || tDstOffset > size || tData.size_bytes() > (size - tDstOffset))
    {
        LogError("Invalid upload range or uninitialized destination buffer.");
        return false;
    }

    auto const staging = Create(
        BufferDescription{
            .size               = tData.size_bytes(),
            .usage              = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .residence          = MemoryResidence::kHost,
            .hostAccess         = HostAccessFlags::kSeqWrite,
            .persistentlyMapped = false,
            .resizable          = false,
        });

    if (!staging || !staging->Write(tData.data(), tData.size_bytes()))
    {
        LogError("Failed to write upload data into staging buffer.");
        return false;
    }

    return CopyBuffer(staging->buffer, buffer, tData.size_bytes(), 0U, tDstOffset);
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
    std::swap(mUsage, tOther.mUsage);
    std::swap(mResizable, tOther.mResizable);
}

}// namespace polos::rendering
