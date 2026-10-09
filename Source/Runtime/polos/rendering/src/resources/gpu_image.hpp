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

struct ImageUse
{
    VkImageLayout        layout{VK_IMAGE_LAYOUT_UNDEFINED};
    VkPipelineStageFlags stages{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};
    VkAccessFlags        accesses{0U};

    /// Conservative defaults; supply explicit scopes for a particular producer/consumer.
    [[nodiscard]] static auto ForLayout(VkImageLayout tLayout) -> ImageUse;
};

class GpuBuffer;

class GpuImage
{
public:
    GpuImage(GpuImage&& tOther);
    GpuImage(GpuImage const&) = delete;

    ~GpuImage();

    auto operator=(GpuImage&& tOther) -> GpuImage&;
    auto operator=(GpuImage const&) -> GpuImage& = delete;


    /// @brief Creates GpuImage instance with an owning VkImage.
    /// @param tDesc parameters for creating a VkImage
    static auto Create(ImageDescription const& tDesc) -> Result<std::unique_ptr<GpuImage>>;

    /// @brief Creates GpuImage without owning the VkImage
    /// @param tImg VkImage to hold for operations.
    static auto Create(ImageDescription const& tDesc, VkImage tImg, ImageUse tInitialUse = {})
        -> Result<std::unique_ptr<GpuImage>>;

    /// @brief Records a image barrier outside
    /// Callers must submit that order on the same queue.
    /// @param tCmdBuf VkCommandBuffer to use
    /// @param tNewUse Precise image usage in the new layout
    [[nodiscard]] auto ChangeLayout(VkCommandBuffer tCmdBuf, ImageUse tNewUse) -> Result<void>;

    /// @brief Records a image barrier outside
    /// Callers must submit that order on the same queue.
    /// @param tCmdBuf VkCommandBuffer to use
    /// @param tNewLayout Assume correct pipeline stage and access masks for the layout
    [[nodiscard]] auto ChangeLayout(VkCommandBuffer tCmdBuf, VkImageLayout tNewLayout) -> Result<void>;

    auto BlitFrom(VkCommandBuffer tCmdBuf, GpuImage& tBlitSrc) -> void;
    auto BlitFrom(VkCommandBuffer tCmdBuf, GpuBuffer& tBlitSrc) -> void;

    /// Track an implicit render-pass transition or externally recorded image use. No barrier.
    auto               SetUse(ImageUse tUse) -> void;
    [[nodiscard]] auto GetUse() const -> ImageUse;

    VmaAllocation allocation{VK_NULL_HANDLE};
    VkImage       img{VK_NULL_HANDLE};
    VkExtent3D    extent{.width = 0U, .height = 0U, .depth = 0U};
    VkFormat      format{VK_FORMAT_UNDEFINED};
private:
    friend class RenderContext;

    GpuImage() = default;

    auto destroy() -> void;
    auto swap(GpuImage& tOther) -> void;

    static VmaAllocator sAllocator;

    ImageUse                mCurrentUse{};
    VkImageSubresourceRange mSubresourceRange{};
    bool                    mOwnsImage{false};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_RESOURCES_GPU_IMAGE_HPP
