#pragma once

#include <glad/glad.h>

#include "polos/graphics/draw_data.h"

namespace polos
{
namespace graphics
{
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

    class DrawStrategy
    {
        using DrawStrategyVariant = std::variant<draw_strategy::DrawIndexed, draw_strategy::DrawIndexedInstanced,
                                                 draw_strategy::DrawArrays, draw_strategy::DrawArraysInstanced>;
    public:
        void Draw(draw_data const& data);
    private:
        friend class DrawStrategyFactory;
        DrawStrategy(DrawStrategyVariant t_strategy_variant);

        DrawStrategyVariant m_strategy_variant;
    };

    class DrawStrategyFactory
    {
    public:
        template<typename T, typename... Ts>
        static DrawStrategy create(Ts&&... args)
        {
            return Strategy(T(std::forward<Ts>(args)...));
        }
    };

}// namespace graphics
}// namespace polos
