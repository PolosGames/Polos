#pragma once
#ifndef POLOS_CORE_ECS_COMPONENTS_COMPONENTTRANSFORM_H_
#define POLOS_CORE_ECS_COMPONENTS_COMPONENTTRANSFORM_H_


namespace polos::ecs
{
    struct transform_component
    {
        glm::vec4 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };
} // namespace polos::ecs

#endif /* POLOS_CORE_ECS_COMPONENTS_COMPONENTTRANSFORM_H_ */