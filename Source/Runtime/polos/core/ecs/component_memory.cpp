
#include "component_memory.h"

#include "polos/core/ecs/components/components.h"
#include "polos/core/engine/engine.h"

namespace polos::ecs
{
    static std::array<std::size_t, k_EntityMaxComponentAmount> GetComponentOffsetArray()
    {
        auto const component_size = base_component::s_ComponentSizeArray;

        std::array<std::size_t, k_EntityMaxComponentAmount> component_offsets{};

        std::size_t cumulative_size = 0;

        for (std::size_t i = 0; i < k_EntityMaxComponentAmount; ++i)
        {
            component_offsets[i] = cumulative_size;
            cumulative_size += component_size[i] * k_SceneMaxEntityAmount;
        }

        return component_offsets;
    }

    ComponentMemory* ComponentMemory::s_Instance;

    ComponentMemory::ComponentMemory()
    {
        SUBSCRIBE_TO_ENGINE_STARTUP(Startup);
        SUBSCRIBE_TO_ENGINE_SHUTDOWN(Shutdown);

        auto const offsets = GetComponentOffsetArray();
        auto const sizes   = base_component::s_ComponentSizeArray;

        k_ComponentOffset.assign(offsets.cbegin(), offsets.cend());
        k_ComponentSizeArray.assign(sizes.cbegin(), sizes.cend());
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
            total_allocation_size += size * k_SceneMaxEntityAmount;
        }
        m_Data = std::malloc(total_allocation_size);
    }

    void ComponentMemory::Shutdown()
    {
        std::free(m_Data);
    }
}// namespace polos::ecs
