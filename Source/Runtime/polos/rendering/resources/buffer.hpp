///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_BUFFER_HPP
#define POLOS_RENDERING_BUFFER_HPP

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <ranges>
#include <span>

namespace polos::rendering
{

enum class MemoryResidence : std::uint8_t
{
    kHost   = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
    kDevice = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,

    kMax = 0x7F,
};

enum class HostAccessFlags : std::uint16_t
{
    kNone     = 0U,
    kSeqWrite = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
    kRandom   = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
};

struct BufferDescription
{
    VkDeviceSize       size{0U};
    VkBufferUsageFlags usage{0U};
    MemoryResidence    residence{MemoryResidence::kDevice};
    HostAccessFlags    hostAccess{HostAccessFlags::kNone};
    bool               persistentlyMapped{true};
    bool               resizable{true};
};

class Buffer
{
public:
    Buffer(Buffer&& tOther);
    Buffer(Buffer const&) = delete;

    ~Buffer();

    auto operator=(Buffer&& tOther) -> Buffer&;
    auto operator=(Buffer const&) -> Buffer& = delete;

    /// @return The allocated buffer, or nullptr if allocation or mapping fails.
    /// @note Adds TRANSFER_SRC and TRANSFER_DST usage when tDesc.resizable is true.
    [[nodiscard]] static auto Create(BufferDescription const& tDesc) -> std::unique_ptr<Buffer>;

    /// @brief Initialize a buffer through a temporary staging allocation. The upload completes before returning.
    /// @return Buffer, or nullptr if the data size differs from tDesc.size or upload fails.
    /// @note Adds TRANSFER_DST usage for the initial upload.
    [[nodiscard]] static auto Create(BufferDescription const& tDesc, std::span<std::byte const> tInitialData) -> std::unique_ptr<Buffer>;

    [[nodiscard]] static auto CreateFixedVertexBuffer(std::span<std::byte const> tData) -> std::unique_ptr<Buffer>;
    [[nodiscard]] static auto CreateFixedIndexBuffer(std::span<std::byte const> tData) -> std::unique_ptr<Buffer>;

    /// Preserves the common prefix and leaves newly added bytes uninitialized.
    /// Finish using the old buffer before resizing. A size change replaces the handle and mapping.
    /// Refresh descriptors that reference the old handle after a successful size change.
    /// Returns false for fixed-size buffers, a zero size, or an allocation or copy failure.
    [[nodiscard]] auto Resize(VkDeviceSize tSize) -> bool;

    /// @brief Upload data into a buffer via a temporary staging allocation. The upload completes before returning.
    /// @note Requires TRANSFER_DST usage (enabled automatically when resizable is true).
    [[nodiscard]] auto Upload(std::span<std::byte const> tData, VkDeviceSize tDstOffset = 0U) -> bool;

    /// @brief Copy data into gpu buffer
    /// @tparam T Data type
    /// @tparam C Contiguous container
    /// @param tData Any container that has contiguous memory and that can be moved into a std::span
    template<typename T, template<typename...> typename C>
        requires std::ranges::contiguous_range<C<T>>
    auto Write(C<T> tData) -> bool
    {
        std::span<T> data = tData;
        return VK_SUCCESS == vmaCopyMemoryToAllocation(sAllocator, data.data(), allocation, 0U, data.size_bytes());
    }

    /// @brief Copy data into gpu buffer
    /// @param tBuffer Raw pointer to data to be copied.
    /// @param tSize Size in total bytes
    template<typename T>
    auto Write(T* tBuffer, std::size_t tSize, size_t tOffset = 0U) -> bool
    {
        return VK_SUCCESS == vmaCopyMemoryToAllocation(sAllocator, tBuffer, allocation, tOffset, tSize);
    }

    static auto SetAllocator(VmaAllocator tAllocator) -> void;

    VmaAllocation   allocation{VK_NULL_HANDLE};
    VkBuffer        buffer{VK_NULL_HANDLE};
    MemoryResidence residence{MemoryResidence::kDevice};
    HostAccessFlags hostAccess{HostAccessFlags::kNone};
    VkDeviceSize    size{0U};
    void*           mapping{nullptr};
private:
    friend class VulkanDevice;

    Buffer() = default;

    auto destroy() -> void;
    auto swap(Buffer& tOther) -> void;

    VkBufferUsageFlags mUsage{0U};
    bool               mResizable{false};

    static VmaAllocator sAllocator;
};

}// namespace polos::rendering

constexpr bool operator&(polos::rendering::HostAccessFlags tLhs, polos::rendering::HostAccessFlags tRhs)
{
    return 0U != (static_cast<std::uint16_t>(tLhs) & static_cast<std::uint16_t>(tRhs));
}

constexpr polos::rendering::HostAccessFlags operator|(polos::rendering::HostAccessFlags tLhs, polos::rendering::HostAccessFlags tRhs)
{
    return static_cast<polos::rendering::HostAccessFlags>(static_cast<std::uint16_t>(tLhs) | static_cast<std::uint16_t>(tRhs));
}

constexpr polos::rendering::HostAccessFlags& operator|=(polos::rendering::HostAccessFlags& tLhs, polos::rendering::HostAccessFlags tRhs)
{
    tLhs = tLhs | tRhs;
    return tLhs;
}

#endif// POLOS_RENDERING_BUFFER_HPP
