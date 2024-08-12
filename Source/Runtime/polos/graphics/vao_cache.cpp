#include "polos/graphics/vao_cache.h"
#include "vao_cache.h"

namespace polos
{
VaoCache* VaoCache::s_Instance;

VaoCache::VaoCache() {}

VaoCache::~VaoCache() {}

uint32 VaoCache::New(std::span<quad_vertex> const, graphics::DrawStrategy const)
{
    return uint32();
}

uint32 VaoCache::New(std::span<quad_vertex> const p_Vertices, std::span<uint32> const p_Indices,
                     graphics::DrawStrategy const p_DrawStrategy)
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

    glEnableVertexArrayAttrib(vao_id, 0);
    glEnableVertexArrayAttrib(vao_id, 1);
    glEnableVertexArrayAttrib(vao_id, 2);
    glEnableVertexArrayAttrib(vao_id, 3);

    glVertexArrayAttribFormat(vao_id, 0, 3, GL_FLOAT, GL_FALSE, offsetof(quad_vertex, position));
    glVertexArrayAttribFormat(vao_id, 1, 3, GL_FLOAT, GL_FALSE, offsetof(quad_vertex, normal));
    glVertexArrayAttribFormat(vao_id, 2, 2, GL_FLOAT, GL_FALSE, offsetof(quad_vertex, texture_coordinates));
    glVertexArrayAttribFormat(vao_id, 3, 4, GL_FLOAT, GL_FALSE, offsetof(quad_vertex, color));

    glVertexArrayAttribBinding(vao_id, 0, 0);
    glVertexArrayAttribBinding(vao_id, 1, 0);
    glVertexArrayAttribBinding(vao_id, 2, 0);
    glVertexArrayAttribBinding(vao_id, 3, 0);


    uint32_t index = static_cast<uint32_t>(m_Cache.size());
    m_Cache.emplace_back(vao_id, buffer_id, static_cast<int32>(p_Indices.size()), ind_offset, true, p_DrawStrategy);

    return index;
}
}// namespace polos
