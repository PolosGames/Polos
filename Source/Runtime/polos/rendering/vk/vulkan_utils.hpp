///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_UTILS_HPP
#define POLOS_RENDERING_VULKAN_UTILS_HPP

#include <vulkan/vulkan.h>

#include <cstdint>
#include <span>
#include <type_traits>


namespace polos::rendering::util
{

auto CreateImageView(VkImage tImage, VkFormat tFormat, VkImageAspectFlags tAspectFlags) -> VkImageView;

auto CreateFramebuffer(VkRenderPass tRPass, std::span<VkImageView> tAttachments, VkExtent2D const& tExtent)
    -> VkFramebuffer;

inline auto To3DExtent(VkExtent2D const& tExtent) -> VkExtent3D
{
    return VkExtent3D{.width = tExtent.width, .height = tExtent.height, .depth = 1U};
}

}// namespace polos::rendering::util


template<typename T>
    requires std::is_integral_v<T>
inline constexpr auto ToVkSize(T const tVar) -> std::uint32_t
{
    return static_cast<std::uint32_t>(tVar);
}

constexpr bool operator==(VkExtent2D tLhs, VkExtent2D tRhs)
{
    return tLhs.width == tRhs.width && tLhs.height == tRhs.height;
}

constexpr bool operator!=(VkExtent2D tLhs, VkExtent2D tRhs)
{
    return !operator==(tLhs, tRhs);
}

constexpr bool operator==(VkExtent3D tLhs, VkExtent3D tRhs)
{
    return tLhs.width == tRhs.width && tLhs.height == tRhs.height && tLhs.depth == tRhs.depth;
}

constexpr bool operator!=(VkExtent3D tLhs, VkExtent3D tRhs)
{
    return !operator==(tLhs, tRhs);
}

constexpr VkExtent2D ToExtent2D(VkExtent3D tSrc)
{
    return {
        .width  = tSrc.width,
        .height = tSrc.height,
    };
}

constexpr VkExtent3D ToExtent3D(VkExtent2D tSrc)
{
    return {
        .width  = tSrc.width,
        .height = tSrc.height,
        .depth  = 1U,
    };
}

#endif// POLOS_RENDERING_VULKAN_UTILS_HPP
