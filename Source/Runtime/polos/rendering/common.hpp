///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_COMMON_HPP
#define POLOS_RENDERING_COMMON_HPP

#include "polos/utils/error_code.hpp"
#include "polos/utils/string_id.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
#include <limits>

#define CHECK_VK_SUCCESS_OR_ERR(Result, Errc) \
    if (VK_SUCCESS != (Result))               \
    {                                         \
        return polos::ErrorType{(Errc)};      \
    }// NOLINT

namespace polos::rendering
{

using MeshHandle     = utils::string_id;
using TextureHandle  = utils::string_id;
using MaterialHandle = utils::string_id;

using MeshIndex    = std::uint32_t;
using TextureIndex = std::uint32_t;

inline constexpr MeshHandle                   kDefaultCube     = "mesh.DefaultCube"_sid;
inline constexpr MeshHandle                   kDefaultQuad     = "mesh.DefaultQuad"_sid;
inline constexpr MaterialHandle               kDefaultMaterial = "material.Default"_sid;
inline constexpr TextureHandle                kDefaultTexture  = "texture.Default"_sid;
inline constexpr TextureIndex                 kDefaultTextureIndex{0U};
inline constexpr std::uint32_t                kRootParentId{0U};
inline constexpr MeshIndex                    kInvalidMeshId{std::numeric_limits<MeshIndex>::max()};
inline constexpr float                        kDefaultTilingFactor{1.0F};
inline constexpr std::array<std::uint8_t, 4U> kDefaultTextureRgba{0xFF, 0x00, 0xFF, 0xFF};// #FF00FFFF

inline constexpr std::uint32_t         kVulkanApiVersion{VK_API_VERSION_1_3};
inline constexpr VkFormat              kSceneColorFormat{VK_FORMAT_R16G16B16A16_SFLOAT};
inline constexpr VkFormat              kSceneDepthFormat{VK_FORMAT_D32_SFLOAT};
inline constexpr VkSampleCountFlagBits kSceneSampleCount{VK_SAMPLE_COUNT_1_BIT};

// Matches local_size_x in main_compute.comp.
inline constexpr std::uint32_t kComputeWorkgroupSize{256U};

}// namespace polos::rendering

namespace polos::rendering::common
{

// Linear RGB for the sRGB brand color #8298A4
constexpr float const kPolosRed{0.22322796F};
constexpr float const kPolosGreen{0.31398871F};
constexpr float const kPolosBlue{0.37123768F};

inline constexpr VkClearValue kClearValue{
    .color = {.float32 = {kPolosRed, kPolosGreen, kPolosBlue, 1.0F}},
};

}// namespace polos::rendering::common

#endif// POLOS_RENDERING_COMMON_HPP
