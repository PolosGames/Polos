///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_COMPILED_RENDER_PASS_HPP
#define POLOS_RENDERING_COMPILED_RENDER_PASS_HPP

#include "polos/rendering/attachment_usage.hpp"

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

#endif// POLOS_RENDERING_COMPILED_RENDER_PASS_HPP
