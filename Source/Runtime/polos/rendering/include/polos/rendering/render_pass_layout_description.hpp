///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_PASS_LAYOUT_DESCRIPTION_HPP
#define POLOS_RENDERING_RENDER_PASS_LAYOUT_DESCRIPTION_HPP

#include <vulkan/vulkan.h>

#include <vector>

namespace polos::rendering
{

struct alignas(128) render_pass_layout_description// NOLINT
{
    std::vector<VkAttachmentDescription2> attachments;
    std::vector<VkSubpassDescription2>    subpasses;
    std::vector<VkSubpassDependency2>     dependencies;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_PASS_LAYOUT_DESCRIPTION_HPP
