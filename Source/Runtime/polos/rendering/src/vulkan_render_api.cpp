//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_api.hpp"

#include "polos/rendering/render_context.hpp"

polos::rendering::IRenderContext* CreateRenderContext()
{
    // Capsulated in PlatformManager
    return new polos::rendering::RenderContext;
}
