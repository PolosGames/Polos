///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/input_state.hpp"

namespace polos::core::input
{

auto IsKeyDown(std::int32_t tKey) -> bool
{ return g_input_state.keysDown[static_cast<std::size_t>(tKey)]; }

auto IsKeyPressed(std::int32_t tKey) -> bool
{
    auto key_loc = static_cast<std::size_t>(tKey);
    return g_input_state.keysDown[key_loc] && !g_input_state.keysDownPrev[key_loc];
}

auto IsKeyReleased(std::int32_t tKey) -> bool
{
    auto key_loc = static_cast<std::size_t>(tKey);
    return !g_input_state.keysDown[key_loc] && g_input_state.keysDownPrev[key_loc];
}


}// namespace polos::core::input