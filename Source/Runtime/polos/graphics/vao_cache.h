#pragma once

#include "polos/graphics/draw_strategy.h"
#include "polos/graphics/vao.h"

namespace polos
{
class VaoCache
{
public:
    VaoCache();

    VaoCache(VaoCache const&)            = delete;
    VaoCache(VaoCache&&)                 = delete;
    VaoCache& operator=(VaoCache const&) = delete;
    VaoCache& operator=(VaoCache&&)      = delete;

    ~VaoCache();

    uint32 New(std::span<quad_vertex> const p_Vertices, graphics::DrawStrategy const p_DrawStrategy);
    uint32 New(std::span<quad_vertex> const p_Vertices, std::span<uint32> const p_Indices,
               graphics::DrawStrategy const p_DrawStrategy);

    std::span<graphics::vertex_array_object> GetCache();
    graphics::vertex_array_object&           GetVao();
private:
    static VaoCache* s_Instance;

    DArray<graphics::vertex_array_object> m_Cache;
};
}// namespace polos