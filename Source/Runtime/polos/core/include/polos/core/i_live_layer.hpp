//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_CORE_INCLUDE_POLOS_CORE_I_LIVE_LAYER_HPP_
#define POLOS_CORE_INCLUDE_POLOS_CORE_I_LIVE_LAYER_HPP_

#include "polos/core/module_macros.hpp"

namespace polos::core
{

class CORE_EXPORT ILiveLayer
{
public:
    ILiveLayer()          = default;
    virtual ~ILiveLayer() = default;

    virtual char const* Name() const = 0;
};

}// namespace polos::core


#endif// POLOS_CORE_INCLUDE_POLOS_CORE_I_LIVE_LAYER_HPP_
