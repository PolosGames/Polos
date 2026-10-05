///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_COMPOSITOR_FRAME_TARGETS_HPP
#define POLOS_RENDERING_SRC_COMPOSITOR_FRAME_TARGETS_HPP

#include "resources/gpu_image.hpp"

#include <vulkan/vulkan.h>

#include <memory>

namespace polos::rendering
{

struct FrameTargets
{
    std::unique_ptr<GpuImage> colorImg;
    std::unique_ptr<GpuImage> depthImg;

    VkImageView colorImgView{VK_NULL_HANDLE};
    VkImageView depthImgView{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_COMPOSITOR_FRAME_TARGETS_HPP
