///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "game.hpp"

#include "helpers.hpp"
#include "polos/input/input_state.hpp"
#include "polos/rendering/debug_draw.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <polos/scene/scene.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>

namespace ctc
{
using namespace polos;

namespace
{
constexpr glm::vec3 kBackdropCenter{0.0F, 10.0F, -10.0F};
constexpr float     kBackdropScale{96.0F};
constexpr float     kBackdropPeriod{24.0F};

}// namespace

void Game::Setup(polos::scene::Scene* tScene)
{
    mScene = tScene;

    auto const camera    = CreateCamera(mScene, ecs::TransformComponent{.position = {0.0F, 10.F, 10.F}}, ecs::CameraType::kOrtho);
    mScene->activeCamera = camera;

    CreateMaterialFromTexture("texture.tetris_block_cyan", "Material_Hero");
    CreateMaterialFromTexture("texture.tetris_block_yellow", "Material_Smashboy");
    CreateMaterialFromTexture("texture.tetris_block_blue", "Material_BlueRicky");
    CreateMaterialFromTexture("texture.tetris_block_orange", "Material_OrangeRicky");
    CreateMaterialFromTexture("texture.tetris_block_red", "Material_RhodeIslandZ");
    CreateMaterialFromTexture("texture.tetris_block_purple", "Material_Teewee");
    CreateMaterialFromTexture("texture.tetris_block_green", "Material_ClevelandZ");

    auto const backdropMat = CreateMaterialFromTexture("texture.tetris_aurora", "Aurora_Backdrop", {}, 1.0F / kBackdropScale);
    auto       gridMat     = CreateMaterialFromTexture("texture.tetris_grid_cell");
    auto       borderMat   = CreateMaterialFromTexture("texture.tetris_grid_border");

    mBackdrop     = CreateQuad(mScene, backdropMat, ecs::TransformComponent{.position = kBackdropCenter, .scale = kBackdropScale}, "Aurora_Backdrop");
    mBackdropTime = 0.0F;

    // Border
    for (uint32_t row{kGridH - 1}; row > 0; --row)
    {
        mBoard[row][0U].entity =
            CreateCube(mScene, ecs::TransformComponent{.position = {-kHalfGridW, static_cast<float>(row), -9.0F}, .scale = 1.0F}, borderMat);
        mBoard[row][kGridW - 1].entity =
            CreateCube(mScene, ecs::TransformComponent{.position = {kHalfGridW - 1, static_cast<float>(row), -9.0F}, .scale = 1.0F}, borderMat);
    }
    for (int32_t col{0}; col < kGridW; ++col)
    {
        mBoard[0U][static_cast<uint32_t>(col)].entity = CreateCube(
            mScene, ecs::TransformComponent{.position = {static_cast<float>(col) - static_cast<float>(kHalfGridW), 0.0F, -9.0F}, .scale = 1.0F}, borderMat);
    }

    for (int32_t row{kGridH - 1}; row > 0; --row)
    {
        for (int32_t col{-kHalfGridW + 1}; col < kHalfGridW - 1; ++col)
        {
            CreateQuad(mScene, gridMat, ecs::TransformComponent{.position = {col, row, -9.5F}, .scale = 1.0F});
        }
    }

    mCurrentTetro = createHero();
}

void Game::Update(float const tDt)
{
    moveBackground(tDt);
    if (mGameOver)
    {
        return;
    }

    if (mPieceRotated)
    {
        rotateCurrentPiece();
    }

    moveCurrentPiece();

    mAccumulatedTime += tDt;
    mMoveInterval = kMoveInterval;
    mSideMovement = 0.0F;
    handleInput();
}

void Game::Render(float const tDt)
{
    //renderOptionsGui();
}

auto Game::createPiece() -> Tetromino
{
    static constexpr std::array factories{
        &Game::createHero,
        &Game::createSmashboy,
        &Game::createBlueRicky,
        &Game::createOrangeRicky,
        &Game::createRhodeIslandZ,
        &Game::createTeewee,
        &Game::createClevelandZ,
    };
    static std::mt19937                        generator{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> pick{0, factories.size() - 1};

    return (this->*factories[pick(generator)])();
}

auto Game::createHero() -> Tetromino
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{-1.5F, 0.5F},
        glm::vec2{-0.5F, 0.5F},
        glm::vec2{0.5F, 0.5F},
        glm::vec2{1.5F, 0.5F},
    };
    auto const center = glm::vec2{kStartPos} - glm::vec2{0.5F, 0.5F};
    return createTetromino(PieceType::kHero, center, pieceLocalPos, "Material_Hero"_sid, "Hero");
}

auto Game::createSmashboy() -> Tetromino
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{-0.5F, 0.5F},
        glm::vec2{0.5F, 0.5F},
        glm::vec2{-0.5F, -0.5F},
        glm::vec2{0.5F, -0.5F},
    };
    auto const center = glm::vec2{kStartPos} - glm::vec2{0.5F, 0.5F};
    return createTetromino(PieceType::kSmashboy, center, pieceLocalPos, "Material_Smashboy"_sid, "Smashboy");
}

auto Game::createBlueRicky() -> Tetromino
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{-1.0F, 1.0F},
        glm::vec2{-1.0F, 0.0F},
        glm::vec2{0.0F, 0.0F},
        glm::vec2{1.0F, 0.0F},
    };
    auto const center = glm::vec2{kStartPos} - glm::vec2{0.0F, 1.0F};
    return createTetromino(PieceType::kBlueRicky, center, pieceLocalPos, "Material_BlueRicky"_sid, "BlueRicky");
}

auto Game::createOrangeRicky() -> Tetromino
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{1.0F, 1.0F},
        glm::vec2{-1.0F, 0.0F},
        glm::vec2{0.0F, 0.0F},
        glm::vec2{1.0F, 0.0F},
    };
    auto const center = glm::vec2{kStartPos} - glm::vec2{0.0F, 1.0F};
    return createTetromino(PieceType::kOrangeRicky, center, pieceLocalPos, "Material_OrangeRicky"_sid, "OrangeRicky");
}

auto Game::createRhodeIslandZ() -> Tetromino
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{-1.0F, 1.0F},
        glm::vec2{0.0F, 1.0F},
        glm::vec2{0.0F, 0.0F},
        glm::vec2{1.0F, 0.0F},
    };
    auto const center = glm::vec2{kStartPos} - glm::vec2{0.0F, 1.0F};
    return createTetromino(PieceType::kRhodeIslandZ, center, pieceLocalPos, "Material_RhodeIslandZ"_sid, "RhodeIslandZ");
}

auto Game::createTeewee() -> Tetromino
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{0.0F, 1.0F},
        glm::vec2{-1.0F, 0.0F},
        glm::vec2{0.0F, 0.0F},
        glm::vec2{1.0F, 0.0F},
    };
    auto const center = glm::vec2{kStartPos} - glm::vec2{0.0F, 1.0F};
    return createTetromino(PieceType::kTeewee, center, pieceLocalPos, "Material_Teewee"_sid, "Teewee");
}

auto Game::createClevelandZ() -> Tetromino
{
    static constexpr std::array<glm::vec2, 4U> pieceLocalPos{
        glm::vec2{0.0F, 1.0F},
        glm::vec2{1.0F, 1.0F},
        glm::vec2{-1.0F, 0.0F},
        glm::vec2{0.0F, 0.0F},
    };
    auto const center = glm::vec2{kStartPos} - glm::vec2{0.0F, 1.0F};
    return createTetromino(PieceType::kClevelandZ, center, pieceLocalPos, "Material_ClevelandZ"_sid, "ClevelandZ");
}

auto Game::createTetromino(
    PieceType                        tType,
    glm::vec2                        tCenter,
    std::array<glm::vec2, 4U> const& tOffsets,
    rendering::MaterialHandle        tMaterial,
    std::string_view                 tName) -> Tetromino
{
    Tetromino tetro;
    tetro.center = tCenter;

    bool const canSpawn = std::ranges::all_of(tOffsets, [&](glm::vec2 offset) {
        auto const position = tCenter + offset;
        auto const row      = static_cast<int32_t>(position.y);
        auto const col      = static_cast<int32_t>(position.x) + kHalfGridW;
        return row >= 0 && row < kGridH && col >= 0 && col < kGridW &&
               mBoard[static_cast<size_t>(row)][static_cast<size_t>(col)].entity == ecs::kInvalidEntityHandle;
    });
    if (!canSpawn)
    {
        mGameOver = true;
        return tetro;
    }

    for (std::size_t i{0}; i < tOffsets.size(); ++i)
    {
        auto& subPiece  = tetro.subpieces[i];
        subPiece.type   = tType;
        subPiece.pos    = tOffsets[i];
        subPiece.entity = CreateCube(
            mScene, ecs::TransformComponent{.position = glm::vec3{tCenter + subPiece.pos, kStartPos.z}}, tMaterial, std::string{tName} + std::to_string(i));
    }

    return tetro;
}

void Game::moveBackground(float const tDt)
{
    mBackdropTime        = std::fmod(mBackdropTime + tDt, kBackdropPeriod);
    float const phase    = (2.0F * std::numbers::pi_v<float> / kBackdropPeriod) * mBackdropTime;
    auto* const backdrop = mScene->GetMutableComponent<ecs::TransformComponent>(mBackdrop);
    backdrop->position   = kBackdropCenter + glm::vec3{2.6F * std::sin(phase), 1.4F * std::cos(phase), 0.0F};
}

void Game::moveCurrentPiece()
{
    glm::vec2 moveVector{0.0F};
    if (mSideMovement != 0.0F)
    {
        if (canMove(mSideMovement, 0.0F))
        {
            moveVector.x = mSideMovement;
        }
    }

    if (!stopDownMove && mAccumulatedTime >= mMoveInterval)
    {
        mAccumulatedTime = 0.0F;
        if (canMove(0.0F, -1.0F))
        {
            moveVector.y = -1.0F;
        }
        else
        {
            auto const& center = mCurrentTetro.center;
            for (auto const& piece : mCurrentTetro.subpieces)
            {

                auto const col = static_cast<size_t>(center.x + piece.pos.x) + kHalfGridW;
                auto const row = static_cast<size_t>(center.y + piece.pos.y);

                mBoard[row][col].entity = piece.entity;
                mBoard[row][col].type   = piece.type;
            }

            checkRowCompletion();

            mCurrentTetro = createPiece();
            return;
        }
    }

    mCurrentTetro.center += moveVector;
    for (auto const& subPiece : mCurrentTetro.subpieces)
    {
        mScene->GetMutableComponent<ecs::TransformComponent>(subPiece.entity)->position += glm::vec3(moveVector, 0.0F);
    }
}

auto Game::canMove(float tHorizontal, float tVertical) -> bool
{
    return std::ranges::all_of(mCurrentTetro.subpieces, [&](auto const& piece) {
        auto const row = static_cast<size_t>(mCurrentTetro.center.y + piece.pos.y + tVertical);
        auto const col = static_cast<size_t>(mCurrentTetro.center.x + piece.pos.x + tHorizontal) + kHalfGridW;
        return mBoard[row][col].entity == ecs::kInvalidEntityHandle;
    });
}

void Game::rotateCurrentPiece()
{

    for (auto& subPiece : mCurrentTetro.subpieces)
    {
        auto* transform = mScene->GetMutableComponent<ecs::TransformComponent>(subPiece.entity);

        auto const rotated = glm::vec2{subPiece.pos.y, -subPiece.pos.x};
        auto const next    = mCurrentTetro.center + rotated;

        auto const col = static_cast<int32_t>(next.x) + kHalfGridW;
        auto const row = static_cast<int32_t>(next.y);

        if (col < 0 || col >= kGridW || row < 0)
        {
            mPieceRotated = false;
            return;
        }

        if (row < kGridH && mBoard[static_cast<size_t>(row)][static_cast<size_t>(col)].entity != ecs::kInvalidEntityHandle)
        {
            mPieceRotated = false;
            return;
        }

        subPiece.pos = rotated;

        transform->position = glm::vec3{next, -9.0F};
    }

    mPieceRotated = false;
}

void Game::handleInput()
{
    if (input::IsKeyDown(GLFW_KEY_DOWN))
    {
        mMoveInterval = kFastMoveInterval;
    }
    if (input::IsKeyPressed(GLFW_KEY_UP))
    {
        mPieceRotated = true;
    }
    if (input::IsKeyPressed(GLFW_KEY_LEFT))
    {
        mSideMovement = -1.0F;
    }
    if (input::IsKeyPressed(GLFW_KEY_RIGHT))
    {
        mSideMovement = 1.0F;
    }
}

void Game::checkRowCompletion()
{
    size_t writeRow{1U};
    for (size_t readRow{1U}; readRow < kGridH; ++readRow)
    {
        auto&      row  = mBoard[readRow];
        bool const full = std::all_of(row.begin() + 1, row.end() - 1, [](SubPiece const& cell) {
            return cell.entity != ecs::kInvalidEntityHandle;
        });

        if (full)
        {
            for (size_t col{1U}; col < kGridW - 1; ++col)
            {
                mScene->RetireEntity(row[col].entity);
                row[col] = {};
            }
            continue;
        }

        if (writeRow != readRow)
        {
            auto const drop = static_cast<float>(readRow - writeRow);
            for (size_t col{1U}; col < kGridW - 1; ++col)
            {
                auto& cell = row[col];
                if (cell.entity != ecs::kInvalidEntityHandle)
                {
                    mScene->GetMutableComponent<ecs::TransformComponent>(cell.entity)->position.y -= drop;
                }
                mBoard[writeRow][col] = cell;
                cell                  = {};
            }
        }

        ++writeRow;
    }
}

void Game::renderOptionsGui()
{
    ImGui::SetNextWindowSize({250.0F, 380.0F});

    ImGui::Begin("Options");
    //ImGui::Checkbox("Show Demo", &mShowDemo);
    //bool right = input::gInputState.keysDown[GLFW_KEY_RIGHT];
    ImGui::Checkbox("Stop movement", &stopDownMove);
    ImGui::End();

    if (mShowDemo)
    {
        static bool close{false};
        ImGui::ShowDemoWindow(&close);
    }
}

auto Game::Tetromino::Right() -> float
{
    float right = subpieces.front().pos.x;

    for (auto const& piece : subpieces)
    {
        right = std::max(right, piece.pos.x);
    }

    return center.x + right;
}

auto Game::Tetromino::Left() -> float
{
    float left = subpieces.front().pos.x;

    for (auto const& piece : subpieces)
    {
        left = std::min(left, piece.pos.x);
    }

    return center.x + left;
}

auto Game::Tetromino::Bottom() -> float
{
    float bottom = subpieces.front().pos.y;

    for (auto const& piece : subpieces)
    {
        bottom = std::min(bottom, piece.pos.y);
    }

    return center.y + bottom;
}

}// namespace ctc
