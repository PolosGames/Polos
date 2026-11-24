///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef CATCHTHECOIN_GAME_HPP
#define CATCHTHECOIN_GAME_HPP

#include "tetro.hpp"

#include <glm/glm.hpp>

#include <array>
#include <random>

namespace ctc
{

class Game
{
public:
    void Setup(polos::scene::Scene* tScene);

    void Update(float tDt);
    void Render(float tDt);
private:
    static constexpr int32_t kGridH{19};
    static constexpr int32_t kGridW{12};
    static constexpr int32_t kHalfGridW{kGridW / 2};

    static constexpr glm::vec3 kStartPos{0.0F, kGridH - 1, -9.0F};
    static constexpr glm::vec3 kHintStartPos{9.0F, (kGridH / 2) + 1, -9.0F};

    static constexpr float kMoveInterval{1.0F};
    static constexpr float kFastMoveInterval{0.1F};

    struct BoardCell
    {
        polos::ecs::EntityHandle entity{polos::ecs::kInvalidEntityHandle};
    };

    auto createPiece() -> Tetromino;

    [[nodiscard]] auto canPlace(glm::vec2 tCenter, std::array<glm::vec2, 4U> const& tOffsets) const -> bool;

    void moveBackground(float tDt);
    auto tryMove(Tetromino& tTetro, glm::vec2 tDelta) -> bool;
    void rotateCurrentPiece();
    void handleInput();
    void updatePieceTransforms(Tetromino& tTetro);
    void lockCurrentPiece();
    void checkRowCompletion();
    void restart();

    static auto toBoardCell(glm::vec2 tPosition) -> glm::ivec2;

    polos::scene::Scene*     mScene{nullptr};
    polos::ecs::EntityHandle mBackdrop{polos::ecs::kInvalidEntityHandle};
    float                    mBackdropTime{0.0F};

    std::array<std::array<BoardCell, kGridW>, kGridH> mBoard;

    std::array<PieceFactory, 7U> mPieceBag{
        &CreateHero,
        &CreateSmashboy,
        &CreateBlueRicky,
        &CreateOrangeRicky,
        &CreateRhodeIslandZ,
        &CreateTeewee,
        &CreateClevelandZ,
    };
    std::mt19937 mGenerator{std::random_device{}()};
    std::size_t  mNextPiece{0U};

    Tetromino mCurrentTetro;
    bool      mGameOver{false};
    float     mAccumulatedTime{0.0F};

    Tetromino mNextTetro;
};

}// namespace ctc

#endif// CATCHTHECOIN_GAME_HPP
