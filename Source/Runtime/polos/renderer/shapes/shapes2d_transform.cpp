#include "polos/polos_pch.h"

#include <glm/gtc/matrix_transform.hpp>

#include "shapes2d_transform.h"

namespace polos
{
    namespace shapes
    {
        void MoveShape2DToPosition(glm::mat4& model_matrix, glm::vec3 position)
        {
            model_matrix = glm::translate(model_matrix, position);
        }

        void MoveShape2DByDelta(glm::mat4& model_matrix, glm::vec3 position_delta)
        {
            model_matrix[3] += glm::vec4{position_delta, 0.0f};
        }

        void RotateShape2D(glm::mat4& model_matrix, glm::vec3 rotation_in_degrees)
        {
            model_matrix = glm::rotate(model_matrix, glm::radians(rotation_in_degrees.z), glm::vec3{0.0f, 0.0f, 1.0f}); // first rotate around z axis.
            model_matrix = glm::rotate(model_matrix, glm::radians(rotation_in_degrees.y), glm::vec3{0.0f, 1.0f, 0.0f}); // then around y axis
        }

        void ScaleShape2D(glm::mat4& model_matrix, glm::vec3 dimensions)
        {
            model_matrix = glm::scale(model_matrix, dimensions);
        }
    }
} // namespace polos