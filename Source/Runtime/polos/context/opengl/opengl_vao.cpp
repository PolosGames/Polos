#include <polos_pch.h>
#ifdef USE_OPENGL

#include <glad/glad.h>

#include "context/vao.h"

namespace polos
{
    Vao::Vao(std::span<vertex const> vertices, std::span<uint32 const> indices)
        : m_IndCount{ static_cast<int32>(indices.size()) }
    {
        int32 alignment { GL_NONE };
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    
        auto const vrt_size { vertices.size_bytes() };
        auto const ind_size { indices.size_bytes()  };
        
        auto const align = [](int64 const length, int64 const alignment) -> int64 {
            const int64  misalignment { length & ( alignment - 1 ) };
            const int64  padding      { (alignment - misalignment) & (alignment - 1) };
            return length + padding;
        };
        
        int64 const vrt_size_aligned{ align(vrt_size, alignment) };
        int64 const ind_size_aligned{ align(ind_size, alignment) };
        
        int64 const vrt_offset{0};
        int64 const ind_offset{vrt_size_aligned};
        m_IndOffset = ind_offset;
        
        glCreateBuffers(1, &m_BufferId);
        glNamedBufferStorage(m_BufferId, vrt_size_aligned + ind_size_aligned, nullptr, GL_DYNAMIC_STORAGE_BIT);
        
        glNamedBufferSubData(m_BufferId, vrt_offset, vrt_size_aligned, vertices.data());
        glNamedBufferSubData(m_BufferId, ind_offset, ind_size_aligned, indices.data());
    
        glCreateVertexArrays(1, &m_VaoId);
        glVertexArrayVertexBuffer(m_VaoId, 0, m_BufferId, vrt_offset, sizeof(vertex));
        glVertexArrayElementBuffer(m_VaoId, m_BufferId);

        glEnableVertexArrayAttrib(m_VaoId, 0);
        glEnableVertexArrayAttrib(m_VaoId, 1);
        glEnableVertexArrayAttrib(m_VaoId, 2);
        glEnableVertexArrayAttrib(m_VaoId, 3);

        glVertexArrayAttribFormat(m_VaoId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, position));
        glVertexArrayAttribFormat(m_VaoId, 1, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, normal));
        glVertexArrayAttribFormat(m_VaoId, 2, 2, GL_FLOAT, GL_FALSE, offsetof(vertex, texture_coord));
        glVertexArrayAttribFormat(m_VaoId, 3, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, color));
    
        glVertexArrayAttribBinding(m_VaoId, 0, 0);
        glVertexArrayAttribBinding(m_VaoId, 1, 0);
        glVertexArrayAttribBinding(m_VaoId, 2, 0);
        glVertexArrayAttribBinding(m_VaoId, 3, 0);
    }
    
    Vao::~Vao()
    {
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteBuffers(1, &m_BufferId);
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