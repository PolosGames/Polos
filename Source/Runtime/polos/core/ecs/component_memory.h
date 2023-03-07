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
        static void Clear(void* offset_ptr);
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
    void ComponentMemory::Clear(void* offset_ptr)
    {
        auto* list = static_cast<T*>(offset_ptr);

        if (static_cast<char*>(offset_ptr) != (static_cast<char*>(s_Instance->m_Data) + s_Instance->k_ComponentOffset[ecs::Component<T>::GetId()]))
        {
            LOG_ENGINE_ERROR("Component array's pointer and it's predefined start don't match.");
        }

        for (size_t i = 0; i < MAX_ENTITY_COUNT_IN_SCENE; i++)
        {
            list[i].~T();
        }
    }
}// namespace polos::ecs