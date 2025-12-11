///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_POLOS_MAIN_HPP
#define POLOS_CORE_POLOS_MAIN_HPP

#include "polos/core/engine.hpp"
#include "polos/core/i_live_layer.hpp"

namespace polos
{
auto CreateApplication(int argc, char** argv) -> polos::core::ILiveLayer*;
}// namespace polos

#ifdef _MSVC_LANG
#    pragma warning(push)
#    pragma warning(disable : 4008)
#endif

int main(int argc, char* argv[])
{
    return polos::core::Engine::Run(polos::CreateApplication(argc, argv));
}

#ifdef _MSVC_LANG
#    pragma warning(pop)
#endif

#endif// POLOS_CORE_POLOS_MAIN_HPP
