//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_CORE_INCLUDE_POLOS_CORE_POLOS_MAIN_HPP_
#define POLOS_CORE_INCLUDE_POLOS_CORE_POLOS_MAIN_HPP_

#include "polos/core/engine.hpp"
#include "polos/core/i_live_layer.hpp"

static polos::core::ILiveLayer* GetLiveLayer(void* t_ptr = nullptr);

#ifdef _MSVC_LANG
#    pragma warning(push)
#    pragma warning(disable : 4008)
#endif

int main(int, char*[])
{
    polos::core::Engine engine;
    std::int32_t        ret_val = engine.Run(GetLiveLayer());
    return ret_val;
}
#ifdef _MSVC_LANG
#    pragma warning(pop)
#endif

#endif// POLOS_CORE_INCLUDE_POLOS_CORE_POLOS_MAIN_HPP_
