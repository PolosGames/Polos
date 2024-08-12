
#include "ecs_component.h"

namespace polos::ecs
{
    std::array<std::size_t, k_EntityMaxComponentAmount> base_component::s_ComponentSizeArray{};
} // namespace polos::ecs