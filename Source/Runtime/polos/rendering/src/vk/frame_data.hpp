///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_FRAME_DATA_HPP
#define POLOS_RENDERING_SRC_VK_FRAME_DATA_HPP

#include <vulkan/vulkan.h>

#include <cstdint>

namespace polos::rendering
{

class GpuImage;

struct FrameData
{
    std::uint32_t             frameSlot{0U};
    std::uint32_t             scImageIndex{0U};// To test if we got a sc image
    VkCommandBuffer           currentCmdBuf{VK_NULL_HANDLE};
    std::shared_ptr<GpuImage> scImage{nullptr};
    VkImageView               scImageView{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_VK_FRAME_DATA_HPP
