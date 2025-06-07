//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_MAIN_HPP
#define POLOS_MAIN_HPP

#include "polos/core/engine.hpp"
#ifdef _MSVC_LANG
#    pragma warning(push)
#    pragma warning(disable : 4008)
#endif

int main(int argc, char* argv[])
{
    polos::core::Engine engine;
    engine.Run();
}
#ifdef _MSVC_LANG
#    pragma warning(pop)
#endif

#endif//POLOS_MAIN_HPP
