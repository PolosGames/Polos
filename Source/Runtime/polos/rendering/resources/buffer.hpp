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

namespace polos::resource::model
{
struct Primitive;
}// namespace polos::resource::model

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
    [[nodiscard]] static auto Create(BufferDescription const& tDesc) -> std::unique_ptr<Buffer>;

    /// @brief Initialize a buffer through a temporary staging allocation. The upload completes before returning.
    /// @return Buffer, or nullptr if the data size differs from tDesc.size or upload fails.
    /// @note Adds TRANSFER_DST usage automatically.
    [[nodiscard]] static auto Create(BufferDescription const& tDesc, std::span<std::byte const> tInitialData)
        -> std::unique_ptr<Buffer>;

    static auto Create(resource::model::Primitive const& tPrim) -> std::unique_ptr<Buffer>;

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
    auto Write(T* tBuffer, std::size_t tSize) -> bool
    {
        return VK_SUCCESS == vmaCopyMemoryToAllocation(sAllocator, tBuffer, allocation, 0U, tSize);
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

    static VmaAllocator sAllocator;
};

}// namespace polos::rendering

constexpr bool operator&(polos::rendering::HostAccessFlags tLhs, polos::rendering::HostAccessFlags tRhs)
{
    return 0U != (static_cast<std::uint16_t>(tLhs) & static_cast<std::uint16_t>(tRhs));
}

constexpr polos::rendering::HostAccessFlags
operator|(polos::rendering::HostAccessFlags tLhs, polos::rendering::HostAccessFlags tRhs)
{
    return static_cast<polos::rendering::HostAccessFlags>(
        static_cast<std::uint16_t>(tLhs) | static_cast<std::uint16_t>(tRhs));
}

constexpr polos::rendering::HostAccessFlags&
operator|=(polos::rendering::HostAccessFlags& tLhs, polos::rendering::HostAccessFlags tRhs)
{
    tLhs = tLhs | tRhs;
    return tLhs;
}

#endif// POLOS_RENDERING_BUFFER_HPP
