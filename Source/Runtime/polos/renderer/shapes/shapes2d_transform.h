#pragma once
#ifndef POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_
#define POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_

#include <glm/glm.hpp>

namespace polos
{
    namespace shapes
    {
        glm::vec4 MoveShape2DToPosition(glm::mat4& model_matrix, glm::vec3 const& position);
        glm::vec4 MoveShape2DByDelta(glm::mat4& model_matrix, glm::vec3 const& position_delta);
        
        void RotateShape2D(glm::mat4& model_matrix, glm::vec3 rotation_in_degrees);
        void RotateShape2D(
            glm::mat4& model_matrix,
            float x_rotation_in_degrees,
            float y_rotation_in_degrees,
            float z_rotation_in_degrees
        );
        
        PL_NODISCARD glm::mat4 ScaleShape2D(glm::mat4& model_matrix, glm::vec2 dimensions);
        PL_NODISCARD glm::mat4 ScaleShape2D(glm::mat4& model_matrix, float x_scale_amount, float y_scale_amount);
    } // namespace shapes
} // namespace polos

#endif /* POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_ */