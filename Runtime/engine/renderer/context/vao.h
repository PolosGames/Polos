#pragma once

#ifndef POLOS_CONTEXT_VBO_H
#define POLOS_CONTEXT_VBO_H

#include <glm/glm.hpp>

#include "utils/alias.h"
#include "vertex.h"
#include "utils/macro_util.h"

namespace polos
{
    class Vao
    {
    public:
        Vao(std::span<vertex const> vertices, std::span<uint32 const> indices);
        ~Vao();
    
        void Bind() const;
        void Unbind() const;
    private:
        uint32 m_VaoId;
        uint32 m_VboId;
        uint32 m_EboId;
    };
}

#endif /* POLOS_CONTEXT_VBO_H */
