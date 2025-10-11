//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAYOUT_DESCRIPTION_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAYOUT_DESCRIPTION_HPP_

#include "polos/rendering/common.hpp"

#include <vector>

namespace polos::rendering
{
struct render_pass_layout_description
{
    std::vector<VkAttachmentDescription2> attachments;
    std::vector<VkSubpassDescription2>    subpasses;
    std::vector<VkSubpassDependency2>     dependencies;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAYOUT_DESCRIPTION_HPP_
