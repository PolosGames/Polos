///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_ENGINE_HPP
#define POLOS_CORE_ENGINE_HPP

#include "polos/core/i_live_layer.hpp"
#include "polos/core/module_macros.hpp"

#include <cstdint>

namespace polos::core
{
class CORE_EXPORT Engine
{
public:
    static std::int32_t Run(ILiveLayer* t_app_layer);
};
}// namespace polos::core

#endif// POLOS_CORE_ENGINE_HPP
