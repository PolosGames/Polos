#include "polos/polos_pch.h"

#include <glm/gtc/matrix_transform.hpp>

#include "shapes2d_transform.h"

namespace polos
{
    namespace shapes
    {
        namespace
        {
            glm::vec3 g_ScaleDimension          = {0.0f, 0.0f, 1.0f};
            constexpr glm::vec3 g_RotationAxisX = {1.0f, 0.0f, 0.0f};
            constexpr glm::vec3 g_RotationAxisY = {0.0f, 1.0f, 0.0f};
            constexpr glm::vec3 g_RotationAxisZ = {0.0f, 0.0f, 1.0f};
        }        

        glm::vec4 MoveShape2DToPosition(glm::mat4& model_matrix, glm::vec3 const& position)
        {
            model_matrix[3] = glm::vec4(position, 1.0f);
            return {position, 1.0f};
        }

        glm::vec4 MoveShape2DByDelta(glm::mat4& model_matrix, glm::vec3 const& position_delta)
        {
            glm::mat4 translation = glm::mat4(1.0f);
            glm::vec4 current_position = model_matrix[3];
            translation = glm::translate(translation, position_delta);
            current_position = translation * current_position;
            model_matrix = model_matrix * translation;

            return current_position;
        }

        void RotateShape2D(glm::mat4& model_matrix, glm::vec3 rotation_in_degrees)
        {
            model_matrix = glm::rotate(model_matrix, glm::radians(rotation_in_degrees.x), g_RotationAxisX);
            model_matrix = glm::rotate(model_matrix, glm::radians(rotation_in_degrees.y), g_RotationAxisY);
            model_matrix = glm::rotate(model_matrix, glm::radians(rotation_in_degrees.z), g_RotationAxisZ);
        }

        void RotateShape2D(glm::mat4& model_matrix, float x_rotation_in_degrees, float y_rotation_in_degrees, float z_rotation_in_degrees)
        {
            model_matrix = glm::rotate(model_matrix, glm::radians(x_rotation_in_degrees), g_RotationAxisX);
            model_matrix = glm::rotate(model_matrix, glm::radians(y_rotation_in_degrees), g_RotationAxisY);
            model_matrix = glm::rotate(model_matrix, glm::radians(z_rotation_in_degrees), g_RotationAxisZ);
        }

        glm::mat4 ScaleShape2D(glm::mat4& model_matrix, glm::vec2 dimensions)
        {
            g_ScaleDimension.x = dimensions.x;
            g_ScaleDimension.y = dimensions.y;
            glm::mat4 scale_matrix = glm::mat4(1.0f);
            scale_matrix = glm::scale(scale_matrix, g_ScaleDimension);
            return model_matrix * scale_matrix;
        }

        glm::mat4 ScaleShape2D(glm::mat4& model_matrix, float x_scale_amount, float y_scale_amount)
        {
            g_ScaleDimension.x     = x_scale_amount;
            g_ScaleDimension.y     = y_scale_amount;
            glm::mat4 scale_matrix = glm::mat4(1.0f);
            scale_matrix           = glm::scale(scale_matrix, g_ScaleDimension);
            return model_matrix * scale_matrix;
        }
    }
} // namespace polos