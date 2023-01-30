#pragma once

#ifndef POLOS_CORE_ECS_COMPONENTMEMORY_H_
#define POLOS_CORE_ECS_COMPONENTMEMORY_H_

#include "polos/core/ecs/component.h"

namespace polos::ecs
{
    class ComponentMemory
    {
    public:
        ComponentMemory();
        ~ComponentMemory();

        void Startup();
        void Shutdown();

        template<typename T>
        static void* Request()
        {
            return static_cast<char*>(s_Instance->m_Data) + (s_Instance->k_ComponentOffset[g_ComponentId<T>]);
        }

        template<typename T>
        static void Clear(void* offset_ptr)
        {
            T* list = static_cast<T*>(offset_ptr);

            if(static_cast<char*>(offset_ptr) != (static_cast<char*>(s_Instance->m_Data) + s_Instance->k_ComponentOffset[g_ComponentId<T>]))
            {
                LOG_ENGINE_ERROR("Component array's pointer and it's predefined start don't match.");
            }

            for (size_t i = 0; i < MAX_ENTITY_COUNT_IN_SCENE; i++)
            {
                list[i].~T();
            }
        }
    private:
        void*  m_Data;

        std::vector<std::size_t> k_ComponentSizeArray;
        std::vector<std::size_t> k_ComponentOffset;
        static ComponentMemory* s_Instance;
    };
} // namespace polos::ecs

#endif /* POLOS_CORE_ECS_COMPONENTMEMORY_H_*/