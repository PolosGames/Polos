#pragma once

#include "polos/core/ecs/ecs_component.h"
#include "polos/graphics/shader.h"

namespace polos::ecs
{
    struct render_component final : Component<render_component>
    {
        Shader* shader{};
        std::array<RenderHandle, k_EntityMaxRenderHandleAmount> renderHandle{};
    };

    template<> inline constexpr int32 k_ComponentId<render_component> = 6;

    template<> void SerializeComponent(std::vector<byte>& p_OutVector, render_component* p_Component);
    template<> render_component DeserializeComponent(std::vector<byte>& p_InVector, std::ptrdiff_t p_Index);
} // namespace polos::ecs
