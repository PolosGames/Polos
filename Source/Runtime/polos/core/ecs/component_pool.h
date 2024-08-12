#pragma once

#include "polos/core/ecs/component_memory.h"
#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    class ComponentPool
    {
    public:
        ComponentPool() = default;
        ComponentPool(ComponentPool const& p_Other) = delete;
        ComponentPool(ComponentPool&& p_Other) noexcept;
        ComponentPool& operator==(ComponentPool const& p_Rhs) = delete;
        ComponentPool& operator==(ComponentPool&& p_Rhs) noexcept;

        template<EcsComponent T>
        void Create();

        template<EcsComponent T>
        void Destroy();

        void* Get(std::size_t p_Index);
        bool IsInitialized();

        DArray<byte> Serialize();
    public:
        std::size_t componentId{};
    private:
        void*       m_Data{};
        std::size_t m_ElemSize{};
    };

    template<EcsComponent T>
    void ComponentPool::Create()
    {
        m_Data      = ComponentMemory::Request<T>();
        m_ElemSize  = sizeof(T);
        componentId = ecs::Component<T>::GetId();
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
