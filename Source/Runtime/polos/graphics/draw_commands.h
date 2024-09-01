#pragma once

#include "polos/graphics/draw_strategy.h"

namespace polos
{
namespace graphics
{
    inline constexpr glm::vec2 kDefaultSize{1.0f, 1.0f};

    void DrawQuad(glm::vec2 const t_position, glm::vec2 const t_size, DrawStrategy auto const t_draw_strategy);
    void DrawQuad(glm::vec3 const& t_position, glm::vec2 const t_size, DrawStrategy auto const t_draw_strategy);
    void DrawQuad(glm::vec2 const t_position, float t_rotation, glm::vec2 const t_size,
                  DrawStrategy auto const t_draw_strategy);
    void DrawQuad(glm::vec3 const& t_position, float t_rotation, glm::vec2 const t_size,
                  DrawStrategy auto const t_draw_strategy);

    void DrawTexture2D(glm::vec2 const t_position, float t_rotation, glm::vec2 const t_size,
                       TextureHandle t_texture_handle, float t_tiling_factor, DrawStrategy auto const t_draw_strategy);
    void DrawTexture2D(glm::vec3 const& t_position, float t_rotation, glm::vec2 const t_size,
                       TextureHandle t_texture_handle, float t_tiling_factor, DrawStrategy auto const t_draw_strategy);
}// namespace graphics
}// namespace polos

#include "polos/graphics/draw_commands.inl.h"
