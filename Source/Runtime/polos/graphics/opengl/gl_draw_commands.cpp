//#include "polos/graphics/draw_commands.h"
//
//namespace polos
//{
//namespace graphics
//{
//    namespace internal
//    {
//        void DrawQuad(glm::mat4 const& t_transform)
//        {
//        
//        }
//    }// namespace internal
//
//    inline constexpr glm::mat4 kIdentityMatrix{1.0f};
//    inline constexpr glm::vec3 kZRotationVector{0.0f, 0.0f, 1.0f};
//
//    void DrawQuad(glm::vec2 const t_position, graphics::DrawStrategy auto const t_draw_strategy)
//    {
//        DrawQuad(glm::vec3{t_position, 0.0f}, t_draw_strategy);
//    }
//
//    void DrawQuad(glm::vec3 const& t_position, graphics::DrawStrategy auto const t_draw_strategy)
//    {
//        glm::mat4 transform = glm::translate(kIdentityMatrix, t_position);
//
//        internal::DrawQuad(transform);
//    }
//
//    void DrawQuad(glm::vec2 const t_position, float t_rotation, graphics::DrawStrategy auto const t_draw_strategy)
//    {
//        DrawQuad(glm::vec3{t_position, 0.0f}, t_rotation, t_draw_strategy);
//    }
//
//    void DrawQuad(glm::vec3 const& t_position, float t_rotation, graphics::DrawStrategy auto const t_draw_strategy)
//    {
//        glm::mat4 transform = glm::translate(kIdentityMatrix, t_position) *
//            glm::rotate(kIdentityMatrix, glm::radians(t_rotation), kZRotationVector);
//
//        internal::DrawQuad(transform);
//    }
//
//    void DrawQuad(glm::vec2 const t_position, glm::vec2 const t_size, graphics::DrawStrategy auto const t_draw_strategy)
//    {
//        DrawQuad(glm::vec3{t_position, 0.0f}, t_size, t_draw_strategy);
//    }
//
//    void DrawQuad(glm::vec3 const& t_position, glm::vec2 const t_size, graphics::DrawStrategy auto const t_draw_strategy)
//    {
//        glm::mat4 transform =
//            glm::translate(kIdentityMatrix, t_position) * glm::scale(kIdentityMatrix, glm::vec3{t_size, 1.0f});
//
//        internal::DrawQuad(transform);
//    }
//
//    void DrawQuad(glm::vec2 const t_position, float t_rotation, glm::vec2 const t_size,
//                  graphics::DrawStrategy auto const t_draw_strategy)
//    {
//        DrawQuad(glm::vec3{t_position, 0.0f}, t_rotation, t_size, t_draw_strategy);
//    }
//
//    void DrawQuad(glm::vec3 const& t_position, float t_rotation, glm::vec2 const t_size,
//                  graphics::DrawStrategy auto const t_draw_strategy)
//    {
//        glm::mat4 transform = glm::translate(kIdentityMatrix, t_position) *
//                              glm::rotate(kIdentityMatrix, glm::radians(t_rotation), kZRotationVector) *
//                              glm::scale(kIdentityMatrix, glm::vec3{t_size, 1.0f});
//
//        internal::DrawQuad(transform);
//    }
//
//}// namespace graphics
//}// namespace polos
