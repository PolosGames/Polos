///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "tetro.hpp"

#include "polos/scene/scene_util.hpp"

#include <string>

namespace ctc
{
using namespace polos;

auto CreateHero(glm::vec2 const tSpawn) -> TetrominoDefinition
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{-1.5F, 0.5F},
        glm::vec2{-0.5F, 0.5F},
        glm::vec2{0.5F, 0.5F},
        glm::vec2{1.5F, 0.5F},
    };
    auto const center = tSpawn - glm::vec2{0.5F, 0.5F};
    return {.type = PieceType::kHero, .center = center, .offsets = pieceLocalPos, .material = "material.Hero"_sid, .name = "Hero"};
}

auto CreateSmashboy(glm::vec2 const tSpawn) -> TetrominoDefinition
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{-0.5F, 0.5F},
        glm::vec2{0.5F, 0.5F},
        glm::vec2{-0.5F, -0.5F},
        glm::vec2{0.5F, -0.5F},
    };
    auto const center = tSpawn - glm::vec2{0.5F, 0.5F};
    return {.type = PieceType::kSmashboy, .center = center, .offsets = pieceLocalPos, .material = "material.Smashboy"_sid, .name = "Smashboy"};
}

auto CreateBlueRicky(glm::vec2 const tSpawn) -> TetrominoDefinition
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{-1.0F, 1.0F},
        glm::vec2{-1.0F, 0.0F},
        glm::vec2{0.0F, 0.0F},
        glm::vec2{1.0F, 0.0F},
    };
    auto const center = tSpawn - glm::vec2{0.0F, 1.0F};
    return {.type = PieceType::kBlueRicky, .center = center, .offsets = pieceLocalPos, .material = "material.BlueRicky"_sid, .name = "BlueRicky"};
}

auto CreateOrangeRicky(glm::vec2 const tSpawn) -> TetrominoDefinition
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{1.0F, 1.0F},
        glm::vec2{-1.0F, 0.0F},
        glm::vec2{0.0F, 0.0F},
        glm::vec2{1.0F, 0.0F},
    };
    auto const center = tSpawn - glm::vec2{0.0F, 1.0F};
    return {.type = PieceType::kOrangeRicky, .center = center, .offsets = pieceLocalPos, .material = "material.OrangeRicky"_sid, .name = "OrangeRicky"};
}

auto CreateRhodeIslandZ(glm::vec2 const tSpawn) -> TetrominoDefinition
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{-1.0F, 1.0F},
        glm::vec2{0.0F, 1.0F},
        glm::vec2{0.0F, 0.0F},
        glm::vec2{1.0F, 0.0F},
    };
    auto const center = tSpawn - glm::vec2{0.0F, 1.0F};
    return {.type = PieceType::kRhodeIslandZ, .center = center, .offsets = pieceLocalPos, .material = "material.RhodeIslandZ"_sid, .name = "RhodeIslandZ"};
}

auto CreateTeewee(glm::vec2 const tSpawn) -> TetrominoDefinition
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{0.0F, 1.0F},
        glm::vec2{-1.0F, 0.0F},
        glm::vec2{0.0F, 0.0F},
        glm::vec2{1.0F, 0.0F},
    };
    auto const center = tSpawn - glm::vec2{0.0F, 1.0F};
    return {.type = PieceType::kTeewee, .center = center, .offsets = pieceLocalPos, .material = "material.Teewee"_sid, .name = "Teewee"};
}

auto CreateClevelandZ(glm::vec2 const tSpawn) -> TetrominoDefinition
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{0.0F, 1.0F},
        glm::vec2{1.0F, 1.0F},
        glm::vec2{-1.0F, 0.0F},
        glm::vec2{0.0F, 0.0F},
    };
    auto const center = tSpawn - glm::vec2{0.0F, 1.0F};
    return {.type = PieceType::kClevelandZ, .center = center, .offsets = pieceLocalPos, .material = "material.ClevelandZ"_sid, .name = "ClevelandZ"};
}

auto CreateTetromino(TetrominoDefinition const& tPiece, float const tZ) -> Tetromino
{
    Tetromino tetro{.type = tPiece.type, .center = tPiece.center, .offsets = tPiece.offsets};
    for (std::size_t i{0}; i < tPiece.offsets.size(); ++i)
    {
        auto const position = glm::vec3{tPiece.center + tPiece.offsets[i], tZ};
        tetro.entities[i]   = scene::Create_TexturedCuboid(
            scene::CuboidDetails{
                .name     = std::string{tPiece.name} + std::to_string(i),
                .position = position,
                .material = tPiece.material,
            });
    }

    return tetro;
}

}// namespace ctc
