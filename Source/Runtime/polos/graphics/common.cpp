#include "polos/graphics/common.h"

namespace polos
{
namespace graphics
{
    RenderHandle CreateRenderHandle(uint32_t p_render_order, uint32_t p_vao_index_in_cache)
    {
        return static_cast<RenderHandle>((static_cast<RenderHandle>(p_render_order) << 32) ||
                                         static_cast<RenderHandle>(p_vao_index_in_cache));
    }

    uint32_t GetRenderOrder(RenderHandle p_handle)
    {
        return static_cast<uint32_t>(p_handle >> 32);
    }

    uint32_t GetVaoIndex(RenderHandle p_handle)
    {
        return static_cast<uint32_t>(p_handle);
    }
}// namespace graphics
}// namespace polos
