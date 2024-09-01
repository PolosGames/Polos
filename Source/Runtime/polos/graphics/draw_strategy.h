#pragma once

#include <glad/glad.h>

#include "polos/graphics/draw_data.h"

namespace polos
{
namespace graphics
{
    enum class DrawStrategyType : uint8_t
    {
        kDrawIndexed,
        kDrawArrays,
        kDrawIndexedInstanced,
        kDrawArraysInstanced
    };

    template<typename T>
    concept DrawStrategy = requires(T t, draw_data const data) {
        {
            t.Draw(data)
        };
    };

    namespace draw_strategy
    {
        class DrawIndexed
        {
        public:
            void Draw(draw_data const& data) const noexcept;
        };

        class DrawArrays
        {
        public:
            void Draw(draw_data const& data) const noexcept;
        };

        class DrawIndexedInstanced
        {
        public:
            void Draw(draw_data const& data) const noexcept;
        };

        class DrawArraysInstanced
        {
        public:
            void Draw(draw_data const& data) const noexcept;
        };
    }// namespace draw_strategy
}// namespace graphics
}// namespace polos
