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
    public:
        static int32 GetId();
    private:
        static int32 s_Id;
    };

    template<typename T>
    concept EcsComponent = std::is_base_of_v<Component<T>, T>;

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

    template<EcsComponent T>
    inline void SerializeComponent(std::vector<byte>& p_OutVector, T* p_Component);

    template<EcsComponent T>
    inline T DeserializeComponent(std::vector<byte>& p_InVector, std::size_t p_Index);

}// namespace polos::ecs
