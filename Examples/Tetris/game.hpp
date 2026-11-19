///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef CATCHTHECOIN_GAME_HPP
#define CATCHTHECOIN_GAME_HPP

#include "polos/ecs/entity.hpp"
#include "polos/rendering/common.hpp"

#include <glm/glm.hpp>

#include <array>
#include <string_view>
namespace polos::scene
{
class Scene;
}// namespace polos::scene

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
    static constexpr int32_t kHalfGridH{kGridH / 2};
    static constexpr int32_t kHalfGridW{kGridW / 2};

    static constexpr glm::vec3 kStartPos{0.0F, kGridH - 1, -9.0F};

    static constexpr float kMoveInterval{1.0F};
    static constexpr float kFastMoveInterval{0.1F};

    enum class PieceType : uint8_t
    {
        kHero,
        kSmashboy,
        kBlueRicky,
        kOrangeRicky,
        kRhodeIslandZ,
        kTeewee,
        kClevelandZ,
    };

    struct SubPiece
    {
        PieceType                type{PieceType::kHero};
        glm::vec2                pos{0.0F};
        polos::ecs::EntityHandle entity{polos::ecs::kInvalidEntityHandle};
    };

    struct Tetromino
    {
        std::array<SubPiece, 4U> subpieces;
        glm::vec2                center{0.0F};

        auto Right() -> float;
        auto Left() -> float;
        auto Bottom() -> float;
    };

    auto createPiece() -> Tetromino;
    auto createHero() -> Tetromino;
    auto createSmashboy() -> Tetromino;
    auto createBlueRicky() -> Tetromino;
    auto createOrangeRicky() -> Tetromino;
    auto createRhodeIslandZ() -> Tetromino;
    auto createTeewee() -> Tetromino;
    auto createClevelandZ() -> Tetromino;
    auto createTetromino(
        PieceType                        tType,
        glm::vec2                        tCenter,
        std::array<glm::vec2, 4U> const& tOffsets,
        polos::rendering::MaterialHandle tMaterial,
        std::string_view                 tName) -> Tetromino;

    void moveBackground(float tDt);
    void moveCurrentPiece();
    auto canMove(float tHorizontal, float tVertical) -> bool;
    void rotateCurrentPiece();
    void handleInput();
    void checkRowCompletion();

    void renderOptionsGui();

    polos::scene::Scene*     mScene{nullptr};
    polos::ecs::EntityHandle mBackdrop{polos::ecs::kInvalidEntityHandle};
    float                    mBackdropTime{0.0F};
    bool                     mShowDemo{false};

    std::array<std::array<SubPiece, kGridW>, kGridH> mBoard;

    Tetromino mCurrentTetro;
    bool      mGameOver{false};
    bool      mPieceRotated{false};
    float     mSideMovement{0.0F};

    float mAccumulatedTime{0.0F};
    float mMoveInterval{kMoveInterval};

    bool stopDownMove{false};
};

}// namespace ctc

#endif// CATCHTHECOIN_GAME_HPP
