///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_INPUT_STATE_HPP
#define POLOS_CORE_INPUT_STATE_HPP

#include "polos/polos_api.hpp"

#include <glm/glm.hpp>

#include <bitset>

namespace polos::core::input
{

struct InputState
{
    std::bitset<512> keys_down;
    std::bitset<512> keys_down_prev;

    glm::vec2 mouse_pos;
    glm::vec2 mouse_delta;
};

extern POLOS_API InputState g_input_state;

[[nodiscard]] auto POLOS_API IsKeyDown(std::int32_t t_key) -> bool;
[[nodiscard]] auto POLOS_API IsKeyPressed(std::int32_t t_key) -> bool;
[[nodiscard]] auto POLOS_API IsKeyReleased(std::int32_t t_key) -> bool;

}// namespace polos::core::input

#endif// POLOS_CORE_INPUT_STATE_HPP
