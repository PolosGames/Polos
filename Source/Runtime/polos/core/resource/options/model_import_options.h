#pragma once

#include "polos/core/resource/import_options.h"

namespace polos::resource
{
    enum class ModelUnpackStrategy
    {
        k_KeepHierarchy,
        k_UnpackEverything
    };

    template<>
    struct import_options<model>
    {
        ModelUnpackStrategy strategy;
    };
} // namespace polos::resource