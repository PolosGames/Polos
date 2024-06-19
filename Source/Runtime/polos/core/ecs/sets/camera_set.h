#pragma once

#include "polos/core/ecs/common_set.h"
#include "polos/core/ecs/components/camera_component.h"
#include "polos/core/ecs/components/transform_component.h"
#include "polos/core/scene/scene_view_iterator.h"
namespace polos::ecs
{
    struct camera_set : CommonSet
    {
        COMMONSET_REGISTER(camera_set, transform_component, camera_component)

        camera_set(ecs::Entity p_Entity, transform_component* p_Transform, camera_component* p_Camera);

        static void UpdateCameraVectors(camera_set& p_Set);
        static void UpdateCameraVectors(transform_component* p_Transform, camera_component* p_Camera);

        glm::vec3 Move(glm::vec3 const& p_Destination);
        glm::vec3 Rotate(glm::vec3 const& p_RotationInDegrees);
        PL_NODISCARD glm::mat4 GetViewMatrix();

        Entity entity;
        transform_component* transformComponent;
        camera_component*    cameraComponent;
    };
} // namespace polos::ecs

