#pragma once

namespace polos::ecs
{
    template<typename T>
    Component<T>::Component()
    {
        s_ComponentSizeArray[k_ComponentId<T>] = sizeof(T);
    }

    template<typename T>
    size_t Component<T>::GetId()
    {
        return k_ComponentId<T>;
    }
} // namespace polos::ecs
