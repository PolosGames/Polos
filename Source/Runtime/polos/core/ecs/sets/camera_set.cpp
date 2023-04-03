
#include "camera_set.h"

namespace polos::ecs
{
    camera_set::camera_set(ecs::Entity p_Entity, ecs::transform_component* p_Transform, ecs::camera_component* p_Camera)
        : entity{p_Entity}
        , transformComponent{p_Transform}
        , cameraComponent{p_Camera}
    {}

    void camera_set::UpdateCameraVectors(ecs::camera_set& p_Set)
    {
        // calculate the new Front vector
        p_Set.cameraComponent->front.x = glm::cos(glm::radians(p_Set.transformComponent->rotation.y)) * glm::cos(glm::radians(p_Set.transformComponent->rotation.x));
        p_Set.cameraComponent->front.y = glm::sin(glm::radians(p_Set.transformComponent->rotation.x));
        p_Set.cameraComponent->front.z = glm::sin(glm::radians(p_Set.transformComponent->rotation.y)) * glm::cos(glm::radians(p_Set.transformComponent->rotation.x));
        p_Set.cameraComponent->front = glm::normalize(p_Set.cameraComponent->front);
        // also re-calculate the Right and Up vector
        // normalize the vectors, because their length gets closer to 0 the
        // more you look up or down which results in slower movement.
        p_Set.cameraComponent->right = glm::normalize(glm::cross(p_Set.cameraComponent->front, p_Set.cameraComponent->worldUp));
        p_Set.cameraComponent->up = glm::normalize(glm::cross(p_Set.cameraComponent->right, p_Set.cameraComponent->front));
    }

    void camera_set::UpdateCameraVectors(transform_component* p_Transform, camera_component* p_Camera)
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x         = glm::cos(glm::radians(p_Transform->rotation.y)) * glm::cos(glm::radians(p_Transform->rotation.x));
        front.y         = glm::sin(glm::radians(p_Transform->rotation.x));
        front.z         = glm::sin(glm::radians(p_Transform->rotation.y)) * glm::cos(glm::radians(p_Transform->rotation.x));
        p_Camera->front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        // normalize the vectors, because their length gets closer to 0 the
        // more you look up or down which results in slower movement.
        p_Camera->right = glm::normalize(glm::cross(p_Camera->front, p_Camera->worldUp));
        p_Camera->up    = glm::normalize(glm::cross(p_Camera->right, p_Camera->front));
    }

    glm::mat4 camera_set::GetViewMatrix()
    {
        return glm::lookAt(transformComponent->position, transformComponent->position + cameraComponent->front, cameraComponent->up);
    }

    glm::vec3 camera_set::Move(glm::vec3 const& p_Destination)
    {
        transformComponent->position = p_Destination;

        return transformComponent->position;
    }

    glm::vec3 camera_set::Rotate(glm::vec3 const& p_RotationInDegrees)
    {
        transformComponent->rotation.x = glm::radians(p_RotationInDegrees.x);
        transformComponent->rotation.y = glm::radians(p_RotationInDegrees.y);
        transformComponent->rotation.z = glm::radians(p_RotationInDegrees.z);
        UpdateCameraVectors(*this);

        return transformComponent->rotation;
    }
} // namespace polos::ecs
