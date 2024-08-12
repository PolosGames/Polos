#pragma once

#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    class ComponentMemory
    {
    public:
        ComponentMemory();
        ~ComponentMemory();

        void Startup();
        void Shutdown();

        template<EcsComponent T>
        static void* Request();

        template<EcsComponent T>
        static void Clear(void* p_OffsetPtr);
    private:
        void*  m_Data{};

        std::vector<std::size_t> k_ComponentSizeArray;
        std::vector<std::size_t> k_ComponentOffset;
        static ComponentMemory* s_Instance;
    };
    
    template<EcsComponent T>
    void* ComponentMemory::Request()
    {
        return static_cast<char*>(s_Instance->m_Data) + (s_Instance->k_ComponentOffset[ecs::Component<T>::GetId()]);
    }
    
    template<EcsComponent T>
    void ComponentMemory::Clear(void* p_OffsetPtr)
    {
        auto* list = static_cast<T*>(p_OffsetPtr);

        if (static_cast<char*>(p_OffsetPtr) != (static_cast<char*>(s_Instance->m_Data) + s_Instance->k_ComponentOffset[static_cast<size_t>(ecs::Component<T>::GetId())]))
        {
            LOG_ENGINE_ERROR("Component array's pointer and it's predefined start don't match.");
        }

        for (size_t i = 0; i < k_SceneMaxEntityAmount; i++)
        {
            list[i].~T();
        }
    }
}// namespace polos::ecs