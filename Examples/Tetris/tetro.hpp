///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef CTC_TETRO_HPP
#define CTC_TETRO_HPP

#include "polos/ecs/entity.hpp"
#include "polos/rendering/common.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstdint>
#include <string_view>

namespace polos::scene
{
class Scene;
}// namespace polos::scene

namespace ctc
{

enum class PieceType : std::uint8_t
{
    kHero,
    kSmashboy,
    kBlueRicky,
    kOrangeRicky,
    kRhodeIslandZ,
    kTeewee,
    kClevelandZ,
};

struct Tetromino
{
    PieceType                                type{PieceType::kHero};
    glm::vec2                                center{0.0F};
    std::array<glm::vec2, 4U>                offsets{};
    std::array<polos::ecs::EntityHandle, 4U> entities{};
};

struct TetrominoDefinition
{
    PieceType                        type;
    glm::vec2                        center;
    std::array<glm::vec2, 4U>        offsets;
    polos::rendering::MaterialHandle material;
    std::string_view                 name;
};

using PieceFactory = TetrominoDefinition (*)(glm::vec2);

auto CreateHero(glm::vec2 tSpawn) -> TetrominoDefinition;
auto CreateSmashboy(glm::vec2 tSpawn) -> TetrominoDefinition;
auto CreateBlueRicky(glm::vec2 tSpawn) -> TetrominoDefinition;
auto CreateOrangeRicky(glm::vec2 tSpawn) -> TetrominoDefinition;
auto CreateRhodeIslandZ(glm::vec2 tSpawn) -> TetrominoDefinition;
auto CreateTeewee(glm::vec2 tSpawn) -> TetrominoDefinition;
auto CreateClevelandZ(glm::vec2 tSpawn) -> TetrominoDefinition;

auto CreateTetromino(polos::scene::Scene* tScene, TetrominoDefinition const& tPiece, float tZ) -> Tetromino;

}// namespace ctc

#endif// CTC_TETRO_HPP
