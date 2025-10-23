//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_COMPILED_RENDER_PASS_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_COMPILED_RENDER_PASS_HPP_

#include "polos/rendering/attachment_usage.hpp"
#include "polos/rendering/common.hpp"

#include <string>

namespace polos::rendering
{

class IRenderPass;

struct compiled_render_pass
{
    std::string name;

    IRenderPass* raw_pass{nullptr};
    bool         is_graphical{true};

    std::vector<attachment_usage>          attachments;
    std::vector<RenderGraphResourceHandle> read_resources;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_COMPILED_RENDER_PASS_HPP_
