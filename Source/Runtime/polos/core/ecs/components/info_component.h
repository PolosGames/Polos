#ifndef  POLOS_CORE_ECS_COMPONENTS_INFO_COMPONENT_H
#define  POLOS_CORE_ECS_COMPONENTS_INFO_COMPONENT_H

namespace polos::ecs
{
    struct info_component
    {
        char name[128];
        bool isSelectedOnEditor;
    };
} // namespace polos::ecs

#endif// POLOS_CORE_ECS_COMPONENTS_INFO_COMPONENT_H
