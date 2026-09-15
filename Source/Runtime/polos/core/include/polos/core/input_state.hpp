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
    std::bitset<512> keysDown;
    std::bitset<512> keysDownPrev;

    glm::vec2 mousePos;
    glm::vec2 mouseDelta;
};

extern POLOS_API InputState g_input_state;

[[nodiscard]] auto POLOS_API IsKeyDown(std::int32_t tKey) -> bool;
[[nodiscard]] auto POLOS_API IsKeyPressed(std::int32_t tKey) -> bool;
[[nodiscard]] auto POLOS_API IsKeyReleased(std::int32_t tKey) -> bool;

}// namespace polos::core::input

#endif// POLOS_CORE_INPUT_STATE_HPP
