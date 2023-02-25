#pragma once

#include "polos/utils/alias.h"
#include "polos/utils/feature.h"
#include "polos/utils/guid.h"
#include "polos/context/graphics_context.h"

namespace polos
{
    struct window_props
    {
        std::string title{};
        int32 width{};
        int32 height{};
        int32 refreshRate{60};
        bool  vsync{};
        bool  fullscreen{};
    private:
        uint8 m_holder[2]{44, 34};
    };
}
