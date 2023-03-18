#pragma once
#ifndef POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_
#define POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_

#include <glm/glm.hpp>

namespace polos
{
    namespace shapes
    {
        glm::vec4 MoveShape2DToPosition(glm::mat4& p_ModelMatrix, glm::vec3 const& p_Position);
        glm::vec4 MoveShape2DByDelta(glm::mat4& p_ModelMatrix, glm::vec3 p_PositionDelta);
        
        void RotateShape2D(glm::mat4& p_ModelMatrix, glm::vec3 p_RotationInDegrees);
        void RotateShape2D(
            glm::mat4& p_ModelMatrix,
            float p_XRotationInDegrees,
            float p_YRotationInDegrees,
            float p_ZRotationInDegrees
        );
        
        PL_NODISCARD glm::mat4 ScaleShape2D(glm::mat4& p_ModelMatrix, glm::vec2 dimensions);
        PL_NODISCARD glm::mat4 ScaleShape2D(glm::mat4& p_ModelMatrix, float x_scale_amount, float y_scale_amount);
    } // namespace shapes
} // namespace polos

#endif /* POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_ */