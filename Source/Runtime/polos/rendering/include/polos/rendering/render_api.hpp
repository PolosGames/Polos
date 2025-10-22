//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_API_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_API_HPP_

#include "polos/rendering/module_macros.hpp"

#include "polos/rendering/i_render_context.hpp"

extern "C"
{
    [[nodiscard]] RENDERING_EXPORT polos::rendering::IRenderContext* CreateRenderContext();
}

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_API_HPP_
