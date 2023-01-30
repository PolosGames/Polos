#include "polos/polos_pch.h"

#include "component_pool.h"

namespace polos::ecs
{
    ComponentPool::ComponentPool(ComponentPool&& other) noexcept
    {
        m_ElemSize = std::exchange(other.m_ElemSize, 0);
        m_Data = std::exchange(other.m_Data, nullptr);
    }

    ComponentPool& ComponentPool::operator==(ComponentPool&& other) noexcept
    {
        if (this == &other) return *this;
        m_ElemSize = std::exchange(other.m_ElemSize, 0);
        m_Data = std::exchange(other.m_Data, nullptr);

        return *this;
    }
    void* ComponentPool::Get(size_t index)
    {
        return static_cast<std::byte*>(m_Data) + (index * m_ElemSize);
    }

    bool ComponentPool::IsInitialized()
    {
        return m_Data != nullptr;
    }
}