#pragma once

namespace polos
{
namespace graphics
{
    void DrawQuad(glm::vec2 const t_position, glm::vec2 const t_size, graphics::DrawStrategy auto const t_draw_strategy)
    {
        DrawQuad(glm::vec3{t_position, 0.0f}, t_size, t_draw_strategy);
    }

    void DrawQuad(glm::vec3 const& t_position, glm::vec2 const t_size,
                  graphics::DrawStrategy auto const t_draw_strategy)
    {
        glm::mat4 transform =
            glm::translate(kIdentityMatrix, t_position) * glm::scale(kIdentityMatrix, glm::vec3{t_size, 1.0f});

        internal::DrawQuad(transform);
    }

    void DrawQuad(glm::vec2 const t_position, float t_rotation, glm::vec2 const t_size,
                  graphics::DrawStrategy auto const t_draw_strategy)
    {
        DrawQuad(glm::vec3{t_position, 0.0f}, t_rotation, t_size, t_draw_strategy);
    }

    void DrawQuad(glm::vec3 const& t_position, float t_rotation, glm::vec2 const t_size,
                  graphics::DrawStrategy auto const t_draw_strategy)
    {
        glm::mat4 transform = glm::translate(kIdentityMatrix, t_position) *
                              glm::rotate(kIdentityMatrix, glm::radians(t_rotation), kZRotationVector) *
                              glm::scale(kIdentityMatrix, glm::vec3{t_size, 1.0f});

        internal::DrawQuad(transform);
    }

    void DrawTexture2D(glm::vec2 const t_position, float t_rotation, glm::vec2 const t_size,
                   TextureHandle t_texture_handle, float t_tiling_factor, DrawStrategy auto const t_draw_strategy)
    {

    }
    void DrawTexture2D(glm::vec3 const& t_position, float t_rotation, glm::vec2 const t_size,
                       TextureHandle t_texture_handle, float t_tiling_factor, DrawStrategy auto const t_draw_strategy)
    {

    }
}// namespace graphics
}// namespace polos
