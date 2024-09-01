#pragma once

#include "polos/core/engine/engine.h"
#include "polos/graphics/quad_vertex.h"

namespace polos
{
namespace graphics
{
    class RenderSystem
    {
    public:
        RenderSystem();

        RenderSystem(RenderSystem const&) = delete;
        RenderSystem(RenderSystem&&) = delete;

        RenderSystem& operator=(RenderSystem const&) = delete;
        RenderSystem& operator=(RenderSystem&&) = delete;

        ~RenderSystem() = default;

        void Startup();
        void Shutdown();

        static void SubmitQuad(quad_vertex const& t_quad_vertex);
    }
} // namespace graphics
} // namespace polos
