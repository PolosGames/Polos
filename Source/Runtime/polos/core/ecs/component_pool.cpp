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
        if (index >= MAX_ENTITY_COUNT_IN_SCENE)
        {
            LOG_ENGINE_ERROR("[ComponentPool::Get] Entity index out of range.");
            return nullptr;
        }
        return static_cast<std::byte*>(m_Data) + (index * m_ElemSize);
    }

    bool ComponentPool::IsInitialized()
    {
        return m_Data != nullptr;
    }

    std::vector<byte> ComponentPool::Serialize()
    {
        std::vector<byte> pool_data;
        pool_data.reserve(
            componentId
          + m_ElemSize
          + (MAX_ENTITY_COUNT_IN_SCENE * m_ElemSize)
        );

        auto push_to_pool_data_as_bytes = [&pool_data](auto number) -> bool {
            static_assert(
                std::is_trivially_constructible_v<decltype(number)>,
                "The passed argument is not trivially constructible, therefore cannot be converted to a byte array!");
            std::size_t i{sizeof(number) * 8};
            std::array<byte, sizeof(number)> byte_array;
            while (i != 0)
            {
                byte_array[(i / 8) - 1] = static_cast<byte>(number >> i);
                i -= 8;
            }

            std::copy(byte_array.begin(), byte_array.end(), std::back_inserter(pool_data));

            return true;
        };

        push_to_pool_data_as_bytes(componentId);
        push_to_pool_data_as_bytes(m_ElemSize);

        byte* data = static_cast<byte*>(m_Data);
        std::copy(
            data,
            std::next(data, MAX_ENTITY_COUNT_IN_SCENE * m_ElemSize),
            std::back_inserter(pool_data)
        );

        return pool_data;
    }
}