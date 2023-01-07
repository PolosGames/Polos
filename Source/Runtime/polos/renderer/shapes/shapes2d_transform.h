#pragma once
#ifndef POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_
#define POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_

#include <glm/glm.hpp>

namespace polos
{
    namespace shapes
    {
        void MoveShape2DToPosition(glm::mat4& model_matrix, glm::vec3 position);
        void MoveShape2DByDelta(glm::mat4& model_matrix, glm::vec3 position_delta);
        void RotateShape2D(glm::mat4& model_matrix, glm::vec3 rotation_in_degrees);
        void ScaleShape2D(glm::mat4& model_matrix, glm::vec3 dimensions);
    } // namespace shapes
} // namespace polos

#endif /* POLOS_RENDERER_SHAPES_SHAPES2DTRANSFORM_H_ */