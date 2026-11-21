///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_APP_POLOS_MAIN_HPP
#define POLOS_APP_POLOS_MAIN_HPP

#include "polos/app/i_live_layer.hpp"
#include "polos/core/engine.hpp"

namespace polos::app
{
auto CreateApplication(int tArgc, char** tArgv) -> ILiveLayer*;
}// namespace polos::app

#ifdef _MSVC_LANG
#    pragma warning(push)
#    pragma warning(disable : 4008)
#endif

int main(int tArgc, char* tArgv[])
{ return polos::core::Engine::Run(polos::app::CreateApplication(tArgc, tArgv)); }

#ifdef _MSVC_LANG
#    pragma warning(pop)
#endif

#endif// POLOS_APP_POLOS_MAIN_HPP
