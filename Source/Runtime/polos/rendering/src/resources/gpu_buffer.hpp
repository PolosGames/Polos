///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_RESOURCES_GPU_BUFFER_HPP
#define POLOS_RENDERING_SRC_RESOURCES_GPU_BUFFER_HPP

#include "polos/communication/error_code.hpp"
#include "resources/buffer_description.hpp"

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <utility>

namespace polos::rendering
{

class GpuBuffer
{
public:
    GpuBuffer(GpuBuffer&& tOther);
    GpuBuffer(GpuBuffer const&) = delete;

    ~GpuBuffer();

    auto operator=(GpuBuffer&& tOther) -> GpuBuffer&;
    auto operator=(GpuBuffer const&) -> GpuBuffer& = delete;

    static auto Create(BufferDescription const& tDesc) -> Result<std::unique_ptr<GpuBuffer>>;

    /// @brief Copy data into gpu buffer
    /// @tparam T Data type
    /// @tparam C Contiguous container
    /// @param tData Any container that has contiguous memory and that can be moved into a std::span
    template<typename T, template<typename...> typename C>
        requires std::ranges::contiguous_range<C<T>>
    auto Write(C<T> tData) -> void
    {
        std::span<T> data = tData;
        vmaCopyMemoryToAllocation(sAllocator, data.data(), allocation, 0U, data.size_bytes());
    }

    /// @brief Copy data into gpu buffer
    /// @param tBuffer Raw pointer to data to be copied.
    /// @param tSize Size in total bytes
    template<typename T>
    auto Write(T* tBuffer, std::size_t tSize)
    { vmaCopyMemoryToAllocation(sAllocator, tBuffer, allocation, 0U, tSize); }

    VmaAllocation   allocation{VK_NULL_HANDLE};
    VkBuffer        buffer{VK_NULL_HANDLE};
    MemoryResidence residence{MemoryResidence::kDevice};
    HostAccessFlags hostAccess{HostAccessFlags::kNone};
    VkDeviceSize    size{0U};
    void*           mapping{nullptr};
private:
    friend class RenderContext;

    GpuBuffer() = default;

    auto destroy() -> void;
    auto swap(GpuBuffer& tOther) -> void;

    static VmaAllocator sAllocator;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_RESOURCES_GPU_BUFFER_HPP
