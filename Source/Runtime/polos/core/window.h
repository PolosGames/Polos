#pragma once

#include "polos/utils/alias.h"
#include "polos/utils/feature.h"

namespace polos
{
    struct alignas(64) window_props
    {
        std::string title{};
        int32 width{};
        int32 height{};
        int32 refreshRate{60};
        bool  vsync{};
        bool  fullscreen{};
    };
}
