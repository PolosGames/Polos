///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_I_LIVE_LAYER_HPP
#define POLOS_CORE_I_LIVE_LAYER_HPP

#include "polos/core/module_macros.hpp"

namespace polos::core
{

class CORE_EXPORT ILiveLayer
{
public:
    virtual ~ILiveLayer() = default;

    virtual auto Create() -> void  = 0;
    virtual auto Destroy() -> void = 0;

    [[nodiscard]] virtual auto Name() const -> char const* = 0;
};

}// namespace polos::core

#endif// POLOS_CORE_I_LIVE_LAYER_HPP
