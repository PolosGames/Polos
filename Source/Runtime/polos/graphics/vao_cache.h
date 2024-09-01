#pragma once

#include "polos/graphics/draw_strategy.h"
#include "polos/graphics/quad_vertex.h"

namespace polos
{
namespace graphics
{
    class VaoCache
    {
    public:
        VaoCache(VaoCache const&)            = delete;
        VaoCache(VaoCache&&)                 = delete;
        VaoCache& operator=(VaoCache const&) = delete;
        VaoCache& operator=(VaoCache&&) = delete;

        ~VaoCache() = default;

        uint32_t New(std::span<quad_vertex> const t_dertices);
        uint32_t New(std::span<quad_vertex> const t_vertices, std::span<uint32_t> const t_indices);

        std::span<uint32_t> GetVaoCache();
        uint32_t            GetVao(RenderHandle const t_render_handle);
    private:
        VaoCache() = default;

        void set_quad_attributes(uint32_t t_vao_id);

        static VaoCache* s_instance;
        DArray<uint32_t> m_cache;
    };
} // namespace graphics
}// namespace polos