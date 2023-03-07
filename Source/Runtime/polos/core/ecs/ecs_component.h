#pragma once

namespace polos::ecs
{
    struct base_component
    {
        static int32 s_ComponentCounter;
        static std::array<std::size_t, MAX_COMPONENT_COUNT_FOR_ENTITY> s_ComponentSizeArray;
    };

    template<typename T>
    class Component : base_component
    {
    public:
        Component();

        static int32 GetId();
    private:
        static int32 s_Id;
    };

    template<typename T>
    int32 Component<T>::s_Id = s_ComponentCounter++;

    template<typename T>
    Component<T>::Component()
    {
        s_ComponentSizeArray[s_Id] = sizeof(T);
    }

    template<typename T>
    int32 Component<T>::GetId()
    {
        return s_Id;
    }

    template<typename T>
    concept EcsComponent = std::is_base_of_v<Component<T>, T>;
} // namespace polos::ecs
