#pragma once

#include "polos/core/ecs/ecs_component.h"

namespace polos::ecs
{
    struct info_component final : Component<info_component>
    {
        CharArray<128> name;
        bool isSelectedOnEditor{};
        bool isVisible{};
    };

    template<> inline constexpr int32 k_ComponentId<info_component> = 0;

    template<> void SerializeComponent(std::vector<byte>& p_OutVector, info_component* p_Component);
    template<> info_component DeserializeComponent(std::vector<byte>& p_InVector, std::size_t p_Index);
} // namespace polos::ecs
