#include "polos/graphics/render_system.h"
#include "render_system.h"

namespace polos
{
namespace graphics
{

    RenderSystem::RenderSystem()
    {
        SUBSCRIBE_TO_ENGINE_STARTUP(Startup);
        SUBSCRIBE_TO_ENGINE_SHUTDOWN(Shutdown);
    }

    void RenderSystem::Startup()
    {
    }

    void RenderSystem::Shutdown()
    {
    }

    void RenderSystem::SubmitQuad(quad_vertex const& t_quad_vertex)
    {
    }

} // namespace graphics
} // namespace polos
