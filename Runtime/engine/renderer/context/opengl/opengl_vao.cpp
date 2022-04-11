#include <polos_pch.h>
#ifdef USE_OPENGL

#include <glad/glad.h>

#include "context/vao.h"

namespace polos
{
    Vao::Vao(std::span<vertex const> vertices, std::span<uint32 const> indices)
        : m_VboId(0), m_EboId(0)
    {
        glCreateVertexArrays(1, &m_VaoId);

        auto vertices_size = static_cast<int32>(vertices.size_bytes());
        glCreateBuffers(1, &m_VboId);
        glNamedBufferData(m_VboId, vertices_size, vertices.data(), GL_STATIC_DRAW);
        glVertexArrayVertexBuffer(m_VboId, 0, m_VboId, 0, sizeof(vertex));
        
        if(!indices.empty())
        {
            auto indices_size = static_cast<int32>(indices.size_bytes());
            glCreateBuffers(1, &m_EboId);
            glNamedBufferData(m_EboId, indices_size, indices.data(), GL_STATIC_DRAW);
            glVertexArrayElementBuffer(m_VaoId, m_EboId);
        }
    
        glEnableVertexArrayAttrib(m_VaoId, 0);
        glEnableVertexArrayAttrib(m_VaoId, 1);
    
        glVertexArrayAttribFormat(m_VaoId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, position));
        glVertexArrayAttribFormat(m_VaoId, 1, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, color));
    
        glVertexArrayAttribBinding(m_VaoId, 0, 0);
        glVertexArrayAttribBinding(m_VaoId, 1, 0);
    }
    
    Vao::~Vao()
    {
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteBuffers(1, &m_VboId);
        glDeleteBuffers(1, &m_EboId);
    }

    void Vao::Bind() const
    {
        glBindVertexArray(m_VaoId);
    }
    
    void Vao::Unbind() const
    {
        glBindVertexArray(0);
    }
}

#endif