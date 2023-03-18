#pragma once

#include "polos/core/ecs/ecs_component.h"
#include "polos/graphics/animation.h"

namespace polos::ecs
{
    struct animator_component final : Component<animator_component>
    {
        int32 currentFrame{};
        int32 fps{};
        int32 frameCounter{};
        bool  isStopped{ false };
        base_animation* currentAnimation{};
    };

    template<> inline constexpr int32 k_ComponentId<animator_component> = 5;

    template<> void               SerializeComponent(std::vector<byte>& p_OutVector, animator_component* p_Component);
    template<> animator_component DeserializeComponent(std::vector<byte>& p_InVector, std::size_t p_Index);
} // namespace polos::ecs
