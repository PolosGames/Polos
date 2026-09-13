///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/input_state.hpp"

namespace polos::core::input
{

auto IsKeyDown(std::int32_t t_key) -> bool
{ return g_input_state.keys_down[static_cast<std::size_t>(t_key)]; }

auto IsKeyPressed(std::int32_t t_key) -> bool
{
    auto key_loc = static_cast<std::size_t>(t_key);
    return g_input_state.keys_down[key_loc] && !g_input_state.keys_down_prev[key_loc];
}

auto IsKeyReleased(std::int32_t t_key) -> bool
{
    auto key_loc = static_cast<std::size_t>(t_key);
    return !g_input_state.keys_down[key_loc] && g_input_state.keys_down_prev[key_loc];
}


}// namespace polos::core::input