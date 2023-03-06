#include "component_memory.h"

#include "polos/core/ecs/components/components.h"

namespace polos::ecs
{
    static constexpr std::array<std::size_t, MAX_COMPONENT_COUNT_FOR_ENTITY> GetComponentSizeArray()
    {
        return {
            sizeof(transform_component), sizeof(texture2d_component), sizeof(info_component), sizeof(camera_component)};
    }

    static constexpr std::array<std::size_t, MAX_COMPONENT_COUNT_FOR_ENTITY> GetComponentOffsetArray()
    {
        auto const component_size = GetComponentSizeArray();

        std::array<std::size_t, MAX_COMPONENT_COUNT_FOR_ENTITY> component_offsets;

        std::size_t cumulative_size = 0;

        for (int i = 0; i < MAX_COMPONENT_COUNT_FOR_ENTITY; ++i)
        {
            component_offsets[i] = cumulative_size;
            cumulative_size += component_size[i] * MAX_ENTITY_COUNT_IN_SCENE;
        }

        return component_offsets;
    }

    ComponentMemory* ComponentMemory::s_Instance;

    ComponentMemory::ComponentMemory()
    {
        constexpr auto offsets = GetComponentOffsetArray();
        constexpr auto sizes   = GetComponentSizeArray();

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
        std::size_t total_allocation_size = 0;
        for (auto const& size : k_ComponentSizeArray)
        {
            total_allocation_size += size * MAX_ENTITY_COUNT_IN_SCENE;
        }
        m_Data = std::malloc(total_allocation_size);
    }

    void ComponentMemory::Shutdown()
    {
        std::free(m_Data);
    }
}// namespace polos::ecs
