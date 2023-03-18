
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

        glm::vec4 MoveShape2DToPosition(glm::mat4& p_ModelMatrix, glm::vec3 const& p_Position)
        {
            p_ModelMatrix[3] = glm::vec4(p_Position, 1.0f);
            return {p_Position, 1.0f};
        }

        glm::vec4 MoveShape2DByDelta(glm::mat4& p_ModelMatrix, glm::vec3 const p_PositionDelta)
        {
            glm::mat4 translation = glm::mat4(1.0f);
            glm::vec4 current_position = p_ModelMatrix[3];
            translation = glm::translate(translation, p_PositionDelta);
            current_position = translation * current_position;
            p_ModelMatrix = p_ModelMatrix * translation;

            return current_position;
        }

        void RotateShape2D(glm::mat4& p_ModelMatrix, glm::vec3 const RotationInDegrees)
        {
            p_ModelMatrix = glm::rotate(p_ModelMatrix, glm::radians(RotationInDegrees.x), g_RotationAxisX);
            p_ModelMatrix = glm::rotate(p_ModelMatrix, glm::radians(RotationInDegrees.y), g_RotationAxisY);
            p_ModelMatrix = glm::rotate(p_ModelMatrix, glm::radians(RotationInDegrees.z), g_RotationAxisZ);
        }

        void RotateShape2D(glm::mat4& p_ModelMatrix, float const p_XRotationInDegrees, float const p_YRotationInDegrees, float const p_ZRotationInDegrees)
        {
            p_ModelMatrix = glm::rotate(p_ModelMatrix, glm::radians(p_XRotationInDegrees), g_RotationAxisX);
            p_ModelMatrix = glm::rotate(p_ModelMatrix, glm::radians(p_YRotationInDegrees), g_RotationAxisY);
            p_ModelMatrix = glm::rotate(p_ModelMatrix, glm::radians(p_ZRotationInDegrees), g_RotationAxisZ);
        }

        glm::mat4 ScaleShape2D(glm::mat4& p_ModelMatrix, glm::vec2 const p_Dimensions)
        {
            g_ScaleDimension.x = p_Dimensions.x;
            g_ScaleDimension.y = p_Dimensions.y;
            glm::mat4 scale_matrix = glm::mat4(1.0f);
            scale_matrix = glm::scale(scale_matrix, g_ScaleDimension);
            return p_ModelMatrix * scale_matrix;
        }

        glm::mat4 ScaleShape2D(glm::mat4& p_ModelMatrix, float const p_XScaleAmount, float const p_YScaleAmount)
        {
            g_ScaleDimension.x     = p_XScaleAmount;
            g_ScaleDimension.y     = p_YScaleAmount;
            glm::mat4 scale_matrix = glm::mat4(1.0f);
            scale_matrix           = glm::scale(scale_matrix, g_ScaleDimension);
            return p_ModelMatrix * scale_matrix;
        }
    }
} // namespace polos