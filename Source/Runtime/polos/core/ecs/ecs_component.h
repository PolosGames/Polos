#pragma once

namespace polos::ecs
{
    struct base_component
    {
        static std::array<std::size_t, k_EntityMaxComponentAmount> s_ComponentSizeArray;
    };

    template<typename T>
    class Component : base_component
    {
    public:
        Component();
    public:
        static size_t GetId();
    };

    template<typename T>
    concept EcsComponent = std::is_base_of_v<Component<T>, T>;

    template<EcsComponent T>
    inline constexpr size_t k_ComponentId{static_cast<size_t>(-1)};

    template<EcsComponent T>
    inline void SerializeComponent(std::vector<byte>& p_OutVector, T* p_Component);

    template<EcsComponent T>
    inline T DeserializeComponent(std::vector<byte>& p_InVector, ptrdiff_t p_Index);

}// namespace polos::ecs

#include "ecs_component.inl.h"
