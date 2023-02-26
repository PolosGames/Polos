#pragma once

#include "polos/core/ecs/component_memory.h"
#include "polos/core/ecs/component.h"

namespace polos::ecs
{
    class ComponentPool
    {
    public:
        ComponentPool() = default;
        ComponentPool(ComponentPool const& other) = delete;
        ComponentPool(ComponentPool&& other) noexcept ;
        ComponentPool& operator==(ComponentPool const& rhs) = delete;
        ComponentPool& operator==(ComponentPool&& rhs) noexcept;

        template<EcsComponent T>
        void Create();

        template<EcsComponent T>
        void Destroy();

        void* Get(std::size_t index);
        bool IsInitialized();
    public:
        int32 componentId{};
    private:
        void* m_Data{};
        std::size_t m_ElemSize{};
    };

    template<EcsComponent T>
    void ComponentPool::Create()
    {
        m_Data      = ComponentMemory::Request<T>();
        m_ElemSize  = sizeof(T);
        componentId = g_ComponentId<T>;
    }

    template<EcsComponent T>
    void ComponentPool::Destroy()
    {
        if(m_Data != nullptr)
        {
            ComponentMemory::Clear<T>(m_Data);
        }
    }

} // namespace polos::ecs
