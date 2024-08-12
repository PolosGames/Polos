#include "polos/graphics/draw_strategy.h"

namespace polos
{
namespace graphics
{
    namespace draw_strategy
    {
        void DrawIndexed::Draw(draw_data const& data) const noexcept
        {
            glDrawElements(data.mode, data.count, data.type, data.indices);
        }

        void DrawArrays::Draw(draw_data const& data) const noexcept
        {
            glDrawArrays(data.mode, data.first, data.count);
        }

        void DrawIndexedInstanced::Draw(draw_data const& data) const noexcept
        {
            glDrawElementsInstanced(data.mode, data.count, data.type, data.indices, data.instance_count);
        }

        void DrawArraysInstanced::Draw(draw_data const& data) const noexcept
        {
            glDrawArraysInstanced(data.mode, data.first, data.count, data.instance_count);
        }
    }// namespace draw_strategy

    void DrawStrategy::Draw(draw_data const& data)
    {
        std::visit([&data](auto const& strategy) -> void { strategy.Draw(data); }, m_strategy_variant);
    }
}// namespace graphics
}// namespace polos
