///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_INPUT_INPUT_STATE_HPP
#define POLOS_INPUT_INPUT_STATE_HPP

#include "polos/polos_api.hpp"

#include <glm/glm.hpp>

#include <bitset>
#include <cstdint>

namespace polos::input
{

struct InputState
{
    std::bitset<512> keysDown;
    std::bitset<512> keysDownPrev;

    glm::vec2 mousePos;
    glm::vec2 mouseDelta;
};

extern POLOS_API InputState gInputState;

/// Register input event handlers during engine startup.
POLOS_API auto Initialize() -> void;

[[nodiscard]] auto POLOS_API IsKeyDown(std::int32_t tKey) -> bool;
[[nodiscard]] auto POLOS_API IsKeyPressed(std::int32_t tKey) -> bool;
[[nodiscard]] auto POLOS_API IsKeyReleased(std::int32_t tKey) -> bool;

}// namespace polos::input

#endif// POLOS_INPUT_INPUT_STATE_HPP
