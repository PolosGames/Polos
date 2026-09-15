///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_COMPOSITOR_FRAME_TARGETS_HPP
#define POLOS_RENDERING_SRC_COMPOSITOR_FRAME_TARGETS_HPP

#include <vulkan/vulkan.h>

#include <cstdint>

namespace polos::rendering
{

struct FrameTargets
{
    VkImage colorImg{VK_NULL_HANDLE};
    VkImage depthImg{VK_NULL_HANDLE};

    VkImageView colorImgView{VK_NULL_HANDLE};
    VkImageView depthImgView{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_COMPOSITOR_FRAME_TARGETS_HPP
