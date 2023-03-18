
#include "ecs_component.h"

namespace polos::ecs
{
    std::array<std::size_t, MAX_COMPONENT_COUNT_FOR_ENTITY> base_component::s_ComponentSizeArray{};
} // namespace polos::ecs