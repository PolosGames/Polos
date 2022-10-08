#pragma once

#ifndef POLOS_CORE_WINDOW_H
#define POLOS_CORE_WINDOW_H

#include "polos/utils/alias.h"
#include "polos/utils/feature.h"
#include "polos/utils/guid.h"
#include "polos/context/graphics_context.h"

namespace polos
{
    struct window_props
    {
        std::string title  = "Polos";
        int32 width        = 1280;
        int32 height       = 720;
        int32 refresh_rate = 60;
        bool vsync         = true;
        bool fullscreen    = false;
    private:
        char m_holder[2] = {44, 34};
    };
}

#endif /* POLOS_CORE_WINDOW_H */
