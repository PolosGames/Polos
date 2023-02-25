#include <polos/polos_pch.h>
#ifdef USE_OPENGL

#include "polos/context/vao.h"

#include <glad/glad.h>

namespace polos
{
    static auto Align(int64 const p_Length, int64 const p_Alignment) -> int64
    {
        int64 const misalignment = p_Length & (p_Alignment - 1);
        int64 const padding      = (p_Alignment - misalignment) & (p_Alignment - 1);
        return p_Length + padding;
    }

    Vao::Vao()
        : bound{false}
    {}

    Vao::Vao(std::span<vertex const> p_Vertices, std::span<uint32 const> p_Indices)
        : m_IndCount{ static_cast<int32>(p_Indices.size()) }
    {
        int32 alignment { GL_NONE };
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    
        auto const vrt_size = static_cast<int64>(p_Vertices.size_bytes());
        auto const ind_size = static_cast<int64>(p_Indices.size_bytes());
        
        int64 const vrt_size_aligned = Align(vrt_size, alignment);
        int64 const ind_size_aligned = Align(ind_size, alignment);
        
        int64 const vrt_offset{};
        int64 const ind_offset{vrt_size_aligned};
        m_IndOffset = ind_offset;
        
        glCreateBuffers(1, &m_BufferId);
        glNamedBufferStorage(m_BufferId, vrt_size_aligned + ind_size_aligned, nullptr, GL_DYNAMIC_STORAGE_BIT);
        
        glNamedBufferSubData(m_BufferId, vrt_offset, vrt_size_aligned, p_Vertices.data());
        glNamedBufferSubData(m_BufferId, ind_offset, ind_size_aligned, p_Indices.data());
    
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
    }

    Vao& Vao::operator=(Vao& rhs) noexcept
    {
        if (&rhs == this)
            return *this;

        m_VaoId = rhs.m_VaoId;
        m_BufferId = rhs.m_BufferId;
        m_IndCount = rhs.m_IndCount;
        m_IndOffset = rhs.m_IndOffset;

        rhs.bound = false;
        bound = true;

        return *this;
    }

    Vao& Vao::operator=(Vao&& rhs) noexcept
    {
        if (&rhs == this)
            return *this;

        m_VaoId = rhs.m_VaoId;
        m_BufferId = rhs.m_BufferId;
        m_IndCount = rhs.m_IndCount;
        m_IndOffset = rhs.m_IndOffset;

        rhs.bound = false;
        bound = true;

        return *this;
    }
    
    Vao::~Vao()
    {
        if (bound)
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
}

#endif