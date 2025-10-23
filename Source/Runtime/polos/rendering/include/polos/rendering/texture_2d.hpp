//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_TEXTURE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_TEXTURE_HPP_

#include "polos/rendering/common.hpp"

namespace polos::rendering
{

struct texture_2d
{
    VkImage               image{VK_NULL_HANDLE};
    VkImageView           view{VK_NULL_HANDLE};
    VkFormat              format{VK_FORMAT_UNDEFINED};
    VkExtent2D            extent{.width = 0, .height = 0};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_TEXTURE_HPP_
