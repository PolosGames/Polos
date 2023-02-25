#pragma once

#include <glm/glm.hpp>

#include "polos/utils/alias.h"
#include "polos/utils/macro_util.h"
#include "polos/context/vertex.h"

namespace polos
{
    class Vao
    {
    public:
        Vao(std::span<vertex const> p_Vertices, std::span<uint32 const> p_Indices);
        ~Vao();
    
        void Bind() const;
        void Unbind() const;
        void Draw() const;
    private:
        uint32 m_VaoId{};
        uint32 m_BufferId{};
        int32  m_IndCount{};
        int64  m_IndOffset{};
    };
}
