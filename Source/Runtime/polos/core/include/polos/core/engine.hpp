//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef CORE_ENGINE_ENGINE_H
#define CORE_ENGINE_ENGINE_H

#include "polos/core/i_live_layer.hpp"
#include "polos/core/module_macros.hpp"

namespace polos::core
{
class CORE_EXPORT Engine
{
public:
    void Run(ILiveLayer* t_app_layer);
};
}// namespace polos::core

#endif//CORE_ENGINE_ENGINE_H
