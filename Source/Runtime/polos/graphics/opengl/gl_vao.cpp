#if defined(USE_OPENGL)

#include "polos/graphics/vao.h"

#include <glad/glad.h>

namespace polos
{

Vao::Vao(std::span<quad_vertex const>, std::span<uint32 const>)
{
}

Vao::~Vao()
{}

// void Vao::SetVertexBufferData(std::span<quad_vertex const> const p_VertexBufferData, int64 const p_StartIndex) const
// {
//     auto const vrt_size   = static_cast<int64>(p_VertexBufferData.size_bytes());
//     auto const vrt_offset = p_StartIndex * static_cast<int64>(sizeof(quad_vertex));

//     glNamedBufferSubData(m_BufferId, vrt_offset, vrt_size, p_VertexBufferData.data());
// }

// void Vao::SetVertexBufferSubData(quad_vertex const& p_VertexData, int64 const p_VertexIndex) const
// {
//     auto const vrt_size   = sizeof(quad_vertex);
//     auto const vrt_offset = p_VertexIndex * static_cast<int64>(sizeof(quad_vertex));

//     glNamedBufferSubData(m_BufferId, vrt_offset, vrt_size, &p_VertexData);
// }
}// namespace polos

#endif