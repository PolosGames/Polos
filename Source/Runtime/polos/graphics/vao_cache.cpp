#include "polos/graphics/vao_cache.h"

#include "polos/graphics/common.h"

namespace polos
{
namespace graphics
{
    VaoCache* VaoCache::s_instance;

    uint32_t VaoCache::New(std::span<quad_vertex> const t_vertices)
    {
        auto const vrt_size = static_cast<int64>(t_vertices.size_bytes());

        constexpr int64 vrt_offset{0};

        uint32_t buffer_id{0};
        glCreateBuffers(1, &buffer_id);

        glNamedBufferStorage(buffer_id, vrt_size, t_vertices.data(), 0);
        
        uint32_t vao_id{0};
        glCreateVertexArrays(1, &vao_id);
        glVertexArrayVertexBuffer(vao_id, 0, buffer_id, vrt_offset, sizeof(quad_vertex));

        set_quad_attributes(vao_id);

        uint32_t index = static_cast<uint32_t>(m_cache.size());
        m_cache.push_back(vao_id);

        return index;
    }

    uint32_t VaoCache::New(std::span<quad_vertex> const p_Vertices, std::span<uint32> const p_Indices)
    {
        auto const vrt_size = static_cast<int64>(p_Vertices.size_bytes());
        auto const ind_size = static_cast<int64>(p_Indices.size_bytes());

        constexpr int64 vrt_offset{0};
        int64 const     ind_offset{vrt_size};

        uint32_t buffer_id{0};
        glCreateBuffers(1, &buffer_id);
        glNamedBufferStorage(buffer_id, vrt_size + ind_size, nullptr, GL_DYNAMIC_STORAGE_BIT);

        glNamedBufferSubData(buffer_id, vrt_offset, vrt_size, p_Vertices.data());
        glNamedBufferSubData(buffer_id, ind_offset, ind_size, p_Indices.data());

        uint32_t vao_id{0};
        glCreateVertexArrays(1, &vao_id);
        glVertexArrayVertexBuffer(vao_id, 0, buffer_id, vrt_offset, sizeof(quad_vertex));
        glVertexArrayElementBuffer(vao_id, buffer_id);

        set_quad_attributes(vao_id);

        uint32_t index = static_cast<uint32_t>(m_cache.size());
        m_cache.push_back(vao_id);

        return index;
    }

    std::span<uint32_t> VaoCache::GetVaoCache()
    {
        return m_cache;
    }

    uint32_t VaoCache::GetVao(RenderHandle const t_render_handle)
    {   
        if (k_InvalidRenderHandle == t_render_handle)
        {
            return static_cast<uint32_t>(-1);
        }

        return m_cache[static_cast<size_t>(GetVaoIndex(t_render_handle))];
    }

    void VaoCache::set_quad_attributes(uint32_t t_vao_id)
    {
        glEnableVertexArrayAttrib(t_vao_id, 0);
        glEnableVertexArrayAttrib(t_vao_id, 1);
        glEnableVertexArrayAttrib(t_vao_id, 2);
        glEnableVertexArrayAttrib(t_vao_id, 3);

        glVertexArrayAttribFormat(t_vao_id, 0, 3, GL_FLOAT, GL_FALSE, offsetof(quad_vertex, position));
        glVertexArrayAttribFormat(t_vao_id, 1, 3, GL_FLOAT, GL_FALSE, offsetof(quad_vertex, normal));
        glVertexArrayAttribFormat(t_vao_id, 2, 2, GL_FLOAT, GL_FALSE, offsetof(quad_vertex, texture_coordinates));
        glVertexArrayAttribFormat(t_vao_id, 3, 4, GL_FLOAT, GL_FALSE, offsetof(quad_vertex, color));

        glVertexArrayAttribBinding(t_vao_id, 0, 0);
        glVertexArrayAttribBinding(t_vao_id, 1, 0);
        glVertexArrayAttribBinding(t_vao_id, 2, 0);
        glVertexArrayAttribBinding(t_vao_id, 3, 0);
    }
}// namespace graphics
}// namespace polos
