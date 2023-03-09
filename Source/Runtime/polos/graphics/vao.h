#pragma once

#include "polos/utils/alias.h"
#include "polos/utils/macro_util.h"
#include "polos/graphics/vertex.h"

namespace polos
{
    class Vao
    {
    public:
        Vao() = default;
        Vao(std::span<vertex const> p_Vertices, std::span<uint32 const> p_Indices);
        ~Vao();

        Vao(Vao const&) = delete;
        Vao(Vao&&) noexcept;

        Vao& operator=(Vao const& p_Rhs) = delete;
        Vao& operator=(Vao&& p_Rhs) noexcept;
    public:
        void Bind() const;
        void Unbind() const;
        void Draw() const;

        void SetVertexBufferData(std::span<vertex const> p_VertexBufferData) const;
        void SetVertexBufferData(std::span<vertex const> p_VertexBufferData, int64 p_StartIndex) const;

        void SetVertexBufferSubData(vertex const& p_VertexData, int64 p_VertexIndex) const;
    private:
        uint32 m_VaoId{};
        uint32 m_BufferId{};
        int32  m_IndCount{};
        int64  m_IndOffset{};
        bool   m_Bound{};
    };
}
