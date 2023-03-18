#if defined(USE_OPENGL)

#include "polos/graphics/vao.h"

#include <glad/glad.h>

namespace polos
{
    Vao::Vao(std::span<vertex const> const p_Vertices, std::span<uint32 const> const p_Indices)
        : m_IndCount{static_cast<int32>(p_Indices.size())}
    {
        auto const vrt_size = static_cast<int64>(p_Vertices.size_bytes());
        auto const ind_size = static_cast<int64>(p_Indices.size_bytes());

        int64 const vrt_offset{};
        int64 const ind_offset{vrt_size};
        m_IndOffset = ind_offset;

        glCreateBuffers(1, &m_BufferId);
        glNamedBufferStorage(m_BufferId, vrt_size + ind_size, nullptr, GL_DYNAMIC_STORAGE_BIT);

        glNamedBufferSubData(m_BufferId, vrt_offset, vrt_size, p_Vertices.data());
        glNamedBufferSubData(m_BufferId, ind_offset, ind_size, p_Indices.data());

        glCreateVertexArrays(1, &m_VaoId);
        glVertexArrayVertexBuffer(m_VaoId, 0, m_BufferId, vrt_offset, sizeof(vertex));
        glVertexArrayElementBuffer(m_VaoId, m_BufferId);

        glEnableVertexArrayAttrib(m_VaoId, 0);
        glEnableVertexArrayAttrib(m_VaoId, 1);
        glEnableVertexArrayAttrib(m_VaoId, 2);
        glEnableVertexArrayAttrib(m_VaoId, 3);

        glVertexArrayAttribFormat(m_VaoId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, position));
        glVertexArrayAttribFormat(m_VaoId, 1, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, normal));
        glVertexArrayAttribFormat(m_VaoId, 2, 2, GL_FLOAT, GL_FALSE, offsetof(vertex, textureCoord));
        glVertexArrayAttribFormat(m_VaoId, 3, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, color));

        glVertexArrayAttribBinding(m_VaoId, 0, 0);
        glVertexArrayAttribBinding(m_VaoId, 1, 0);
        glVertexArrayAttribBinding(m_VaoId, 2, 0);
        glVertexArrayAttribBinding(m_VaoId, 3, 0);
        m_Bound = true;
    }

    Vao::Vao(Vao&& p_Rhs) noexcept
        : m_VaoId{std::exchange(p_Rhs.m_VaoId, 0)}
        , m_BufferId{std::exchange(p_Rhs.m_BufferId, 0)}
        , m_IndCount{std::exchange(p_Rhs.m_IndCount, 0)}
        , m_IndOffset{std::exchange(p_Rhs.m_IndOffset, 0)}
        , m_Bound{std::exchange(p_Rhs.m_Bound, false)}
    {}

    Vao& Vao::operator=(Vao&& p_Rhs) noexcept
    {
        if (&p_Rhs == this)
            return *this;

        m_VaoId     = std::exchange(p_Rhs.m_VaoId, 0);
        m_BufferId  = std::exchange(p_Rhs.m_BufferId, 0);
        m_IndCount  = std::exchange(p_Rhs.m_IndCount, 0);
        m_IndOffset = std::exchange(p_Rhs.m_IndOffset, 0);
        m_Bound     = std::exchange(p_Rhs.m_Bound, false);

        return *this;
    }

    Vao::~Vao()
    {
        if (m_Bound)
        {
            glDeleteVertexArrays(1, &m_VaoId);
            glDeleteBuffers(1, &m_BufferId);
        }
    }

    void Vao::Bind() const
    {
        glBindVertexArray(m_VaoId);
    }
    
    void Vao::Unbind() const
    {
        glBindVertexArray(0);
    }
    
    void Vao::Draw() const
    {
        Bind();
        glDrawElements(GL_TRIANGLES, m_IndCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(m_IndOffset));
    }

    void Vao::SetVertexBufferData(std::span<vertex const> const p_VertexBufferData) const
    {
        SetVertexBufferData(p_VertexBufferData, 0);
    }

    void Vao::SetVertexBufferData(std::span<vertex const> const p_VertexBufferData, int64 const p_StartIndex) const
    {
        auto const  vrt_size         = static_cast<int64>(p_VertexBufferData.size_bytes());
        int64 const vrt_offset       = p_StartIndex * sizeof(vertex);

        glNamedBufferSubData(m_BufferId, vrt_offset, vrt_size, p_VertexBufferData.data());
    }

    void Vao::SetVertexBufferSubData(vertex const& p_VertexData, int64 p_VertexIndex) const
    {
        auto const  vrt_size         = sizeof(vertex);
        int64 const vrt_offset       = p_VertexIndex * sizeof(vertex);

        glNamedBufferSubData(m_BufferId, vrt_offset, vrt_size, &p_VertexData);
    }
}

#endif