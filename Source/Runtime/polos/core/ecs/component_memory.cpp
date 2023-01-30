#include "polos/polos_pch.h"

#include "component_memory.h"

#include "polos/core/ecs/components/components.h"

namespace polos::ecs
{
    static constexpr std::array<std::size_t, MAX_COMPONENT_COUNT_FOR_ENTITY> GetComponentSizeArray()
    {
        return {
            sizeof(transform_component),
            sizeof(texture2d_component),
            sizeof(info_component)
        };
    }

    static constexpr std::array<std::size_t, MAX_COMPONENT_COUNT_FOR_ENTITY> GetComponentOffsetArray()
    {
        auto const l_ComponentSize = GetComponentSizeArray();

        std::array<std::size_t, MAX_COMPONENT_COUNT_FOR_ENTITY> l_ComponentOffsets;

        std::size_t l_CumulativeSize = 0;

        for(int i = 0; i < MAX_COMPONENT_COUNT_FOR_ENTITY; i++)
        {
            l_ComponentOffsets[i] = l_CumulativeSize;
            l_CumulativeSize += l_ComponentSize[i] * MAX_ENTITY_COUNT_IN_SCENE;
        }

        return l_ComponentOffsets;
    }

    ComponentMemory* ComponentMemory::s_Instance;

    ComponentMemory::ComponentMemory()
    {
        auto const offsets = GetComponentOffsetArray();
        auto const sizes = GetComponentSizeArray();

        k_ComponentOffset.assign(offsets.begin(), offsets.end());
        k_ComponentSizeArray.assign(sizes.begin(), sizes.end());
        s_Instance = this;
    }

    ComponentMemory::~ComponentMemory()
    {
        s_Instance = nullptr;
    }

    void ComponentMemory::Startup()
    {
        std::size_t l_TotalAllocationSize = 0;
        for(auto const& size : k_ComponentSizeArray)
        {
            l_TotalAllocationSize += size * MAX_ENTITY_COUNT_IN_SCENE;
        }
        m_Data = std::malloc(l_TotalAllocationSize);
    }

    void ComponentMemory::Shutdown()
    {
        std::free(m_Data);
    }
} // namespace polos::ecs
