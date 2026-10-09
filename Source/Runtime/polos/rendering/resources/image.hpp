///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_IMAGE_HPP
#define POLOS_RENDERING_IMAGE_HPP

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>
#include <span>

namespace polos::rendering
{

struct ImageDescription
{
    VkExtent3D            extent{.width = 0U, .height = 0U, .depth = 1U};
    VkFormat              format{VK_FORMAT_UNDEFINED};
    VkImageUsageFlags     usage{0U};
    std::uint32_t         mipLevels{1U};
    std::uint32_t         arrayLayers{1U};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
};

struct ImageUse
{
    VkImageLayout        layout{VK_IMAGE_LAYOUT_UNDEFINED};
    VkPipelineStageFlags stages{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};
    VkAccessFlags        accesses{0U};

    /// Defaults for layouts
    [[nodiscard]] static auto ForLayout(VkImageLayout tLayout) -> ImageUse;
};

class Buffer;

class Image
{
public:
    ~Image();
    Image(Image&& tOther);
    auto operator=(Image&& tOther) -> Image&;

    Image(Image const&)                    = delete;
    auto operator=(Image const&) -> Image& = delete;

    /// @brief Creates Image instance with an owning VkImage.
    /// @param tDesc parameters for creating a VkImage
    /// @return The allocated image, or nullptr if allocation fails.
    [[nodiscard]] static auto Create(ImageDescription const& tDesc) -> std::unique_ptr<Image>;

    /// @brief Synchronously uploads tightly packed pixels and transitions to shader-read-only layout.
    /// @param tDesc Nonzero extent, one mip, one layer, one sample, TRANSFER_DST | SAMPLED usage.
    /// @param tFormat Format of the tPixelData array
    /// @param tPixelData At least width * height * depth * texel-size bytes, in the specified VkFormat.
    /// @return The uploaded image, or nullptr for invalid input or allocation/upload failure.
    [[nodiscard]] static auto
    Create(ImageDescription const& tDesc, VkFormat tFormat, std::span<std::uint8_t const> tPixelData)
        -> std::unique_ptr<Image>;

    /// @brief Creates Image without owning the VkImage
    /// @param tImg VkImage to hold for operations.
    /// @return The image wrapper, or nullptr if tImg is VK_NULL_HANDLE.
    [[nodiscard]] static auto Create(ImageDescription const& tDesc, VkImage tImg, ImageUse tInitialUse = {})
        -> std::unique_ptr<Image>;

    /// @brief Records a image barrier outside
    /// Callers must submit that order on the same queue.
    /// @param tCmdBuf VkCommandBuffer to use
    /// @param tNewUse Precise image usage in the new layout
    auto ChangeLayout(VkCommandBuffer tCmdBuf, ImageUse tNewUse) -> void;

    /// @brief Records a image barrier outside
    /// Callers must submit that order on the same queue.
    /// @param tCmdBuf VkCommandBuffer to use
    /// @param tNewLayout Assume correct pipeline stage and access masks for the layout
    auto ChangeLayout(VkCommandBuffer tCmdBuf, VkImageLayout tNewLayout) -> void;

    auto BlitFrom(VkCommandBuffer tCmdBuf, Image& tBlitSrc) -> void;
    auto BlitFrom(VkCommandBuffer tCmdBuf, Buffer& tBlitSrc) -> void;

    /// Track an implicit render-pass transition or externally recorded image use. No barrier.
    auto               SetUse(ImageUse tUse) -> void;
    [[nodiscard]] auto GetUse() const -> ImageUse;

    static auto SetAllocator(VmaAllocator tAllocator) -> void;

    VmaAllocation           allocation{VK_NULL_HANDLE};
    VkImage                 img{VK_NULL_HANDLE};
    VkExtent3D              extent{.width = 0U, .height = 0U, .depth = 1U};
    VkFormat                format{VK_FORMAT_UNDEFINED};
    VkImageSubresourceRange subresourceRange{};
private:
    Image() = default;

    auto destroy() -> void;
    auto swap(Image& tOther) -> void;

    static VmaAllocator sAllocator;

    ImageUse mCurrentUse{};
    bool     mOwnsImage{false};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_IMAGE_HPP
