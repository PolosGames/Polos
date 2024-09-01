#pragma once

namespace polos
{
namespace graphics
{
    inline constexpr uint64_t k_InvalidRenderHandle{18446744073709551615ULL};

    RenderHandle CreateRenderHandle(uint32_t p_render_order, uint32_t p_vao_index_in_cache);
    uint32_t     GetRenderOrder(RenderHandle p_handle);
    uint32_t     GetVaoIndex(RenderHandle p_handle);
}
}// namespace polos
