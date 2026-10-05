///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_RESOURCES_GPU_IMAGE_HPP
#define POLOS_RENDERING_SRC_RESOURCES_GPU_IMAGE_HPP

#include "polos/communication/error_code.hpp"
#include "resources/image_description.hpp"

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <memory>

namespace polos::rendering
{

class GpuImage
{
public:
    GpuImage(GpuImage&& tOther);
    GpuImage(GpuImage const&) = delete;

    ~GpuImage();

    auto operator=(GpuImage&& tOther) -> GpuImage&;
    auto operator=(GpuImage const&) -> GpuImage& = delete;

    static auto Create(ImageDescription const& tDesc) -> Result<std::unique_ptr<GpuImage>>;

    VmaAllocation allocation{VK_NULL_HANDLE};
    VkImage       img{VK_NULL_HANDLE};
    VkExtent3D    extent;
    VkFormat      format{VK_FORMAT_UNDEFINED};
private:
    friend class RenderContext;

    GpuImage() = default;

    auto destroy() -> void;
    auto swap(GpuImage& tOther) -> void;

    static VmaAllocator sAllocator;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_RESOURCES_GPU_IMAGE_HPP
