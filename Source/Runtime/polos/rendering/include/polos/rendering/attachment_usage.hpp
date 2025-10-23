//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_ATTACHMENT_USAGE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_ATTACHMENT_USAGE_HPP_

#include "polos/rendering/common.hpp"

namespace polos::rendering
{

struct attachment_usage
{
    RenderGraphResourceHandle handle;
    VkAttachmentLoadOp        load_op;
    VkAttachmentStoreOp       store_op;
    VkClearValue              clear_value;
};

}// namespace polos::rendering
#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_ATTACHMENT_USAGE_HPP_
