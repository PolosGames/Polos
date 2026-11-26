///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "game.hpp"

#include "polos/input/input_state.hpp"

#include <imgui.h>
#include <polos/scene/scene.hpp>
#include <polos/scene/scene_util.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <numbers>

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

    auto const camera = scene::Create_Camera(
        scene::CameraDetails{
            .name       = "Camera",
            .position   = {0.0F, 10.F, 10.F},
            .cameraType = polos::ecs::CameraType::kPerspective,
            .nearZ      = 1.0F,
            .farZ       = 30.0F,
            // .size       = 32.0F,
        });
    mScene->activeCamera = camera;

    constexpr auto backdropMat = "material.Aurora_Backdrop"_sid;
    constexpr auto borderMat   = "material.GridBorder"_sid;

    mBackdrop = scene::Create_TexturedQuad(
        scene::QuadDetails{
            .name     = "Background",
            .position = kBackdropCenter,
            .scale    = kBackdropScale,
            .material = backdropMat,
        });
    mBackdropTime = 0.0F;

    // Border
    for (uint32_t row{kGridH - 1}; row > 0; --row)
    {
        mBoard[row][0U].entity = scene::Create_TexturedCuboid(
            scene::CuboidDetails{
                .name     = "LeftBorder",
                .position = {-kHalfGridW, static_cast<float>(row), -9.0F},
                .material = borderMat,
            });
        mBoard[row][kGridW - 1].entity = scene::Create_TexturedCuboid(
            scene::CuboidDetails{
                .name     = "RightBorder",
                .position = {kHalfGridW - 1, static_cast<float>(row), -9.0F},
                .material = borderMat,
            });
    }
    for (int32_t col{0}; col < kGridW; ++col)
    {
        mBoard[0U][static_cast<uint32_t>(col)].entity = scene::Create_TexturedCuboid(
            scene::CuboidDetails{
                .name     = "BottomBorder",
                .position = {static_cast<float>(col) - static_cast<float>(kHalfGridW), 0.0F, -9.0F},
                .material = borderMat,
            });
    }

    scene::Create_TexturedQuad(
        scene::QuadDetails{
            .name         = "Tetris Grid",
            .dimensions   = {static_cast<float>(kGridW - 2), static_cast<float>(kGridH - 1)},
            .position     = {-0.5F, static_cast<float>(kGridH) / 2.0F, -9.5F},
            .material     = "material.GridCell"_sid,
            .tilingFactor = {static_cast<float>(kGridW - 2), static_cast<float>(kGridH - 1)},
        });

    restart();
}

void Game::Update(float const tDt)
{
    moveBackground(tDt);
    if (mGameOver)
    {
        if (input::IsKeyPressed(GLFW_KEY_R))
        {
            restart();
        }
        return;
    }

    handleInput();

    mAccumulatedTime += tDt;
    auto const interval = input::IsKeyDown(GLFW_KEY_DOWN) ? kFastMoveInterval : kMoveInterval;
    if (mAccumulatedTime >= interval)
    {
        mAccumulatedTime = 0.0F;
        if (!tryMove(mCurrentTetro, {0.0F, -1.0F}))
        {
            lockCurrentPiece();
        }
    }
}

void Game::Render(float /*tDt*/)
{
    if (mGameOver)
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, {0.5F, 0.5F});
        if (ImGui::Begin(
                "Game Over", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove))
        {
            float oldSize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 2;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextUnformatted("Game Over");
            ImGui::TextUnformatted("Press R to restart");

            ImGui::GetFont()->Scale = oldSize;
            ImGui::PopFont();
        }
        ImGui::End();
    }
}

auto Game::createPiece() -> Tetromino
{
    if (mNextPiece == mPieceBag.size())
    {
        std::ranges::shuffle(mPieceBag, mGenerator);
        mNextPiece = 0U;
    }

    auto const piece = mPieceBag[mNextPiece++](glm::vec2{kHintStartPos});

    Tetromino currentTet = mNextTetro;

    if (!tryMove(currentTet, glm::vec2(kStartPos - kHintStartPos)))
    {
        mGameOver = true;
        return {};
    }

    mNextTetro = CreateTetromino(piece, kHintStartPos.z);

    return currentTet;
}

auto Game::toBoardCell(glm::vec2 tPosition) -> glm::ivec2
{
    return glm::ivec2{tPosition} + glm::ivec2{kHalfGridW, 0};
}

void Game::moveBackground(float const tDt)
{
    mBackdropTime           = std::fmod(mBackdropTime + tDt, kBackdropPeriod);
    float const phase       = (2.0F * std::numbers::pi_v<float> / kBackdropPeriod) * mBackdropTime;
    auto*       bgTransform = scene::GetComponent_Transform(mBackdrop);
    bgTransform->position   = kBackdropCenter + glm::vec3{2.6F * std::sin(phase), 1.4F * std::cos(phase), 0.0F};
}

auto Game::canPlace(glm::vec2 tCenter, std::array<glm::vec2, 4U> const& tOffsets) const -> bool
{
    return std::ranges::all_of(tOffsets, [&](glm::vec2 offset) {
        auto const cell = toBoardCell(tCenter + offset);
        auto const row  = static_cast<std::size_t>(cell.y);
        auto const col  = static_cast<std::size_t>(cell.x);
        return cell.x >= 0 && cell.x < kGridW && cell.y >= 0 && cell.y < kGridH && mBoard[row][col].entity == ecs::kInvalidEntityHandle;
    });
}

auto Game::tryMove(Tetromino& tTetro, glm::vec2 tDelta) -> bool
{
    auto const center = tTetro.center + tDelta;
    if (!canPlace(center, tTetro.offsets))
    {
        return false;
    }

    tTetro.center = center;
    updatePieceTransforms(tTetro);
    return true;
}

void Game::rotateCurrentPiece()
{
    if (mCurrentTetro.type == PieceType::kSmashboy)
    {
        return;
    }

    auto offsets = mCurrentTetro.offsets;
    for (auto& offset : offsets)
    {
        offset = {offset.y, -offset.x};
    }

    constexpr std::array kicks{
        glm::vec2{0, 0},
        glm::vec2{-1, 0},
        glm::vec2{1, 0},
        glm::vec2{-2, 0},
        glm::vec2{2, 0},
        glm::vec2{0, 1},
        glm::vec2{0, -1},
        glm::vec2{0, 2},
        glm::vec2{0, -2},
    };
    for (auto const kick : kicks)
    {
        auto const center = mCurrentTetro.center + kick;
        if (canPlace(center, offsets))
        {
            mCurrentTetro.center  = center;
            mCurrentTetro.offsets = offsets;
            updatePieceTransforms(mCurrentTetro);
            return;
        }
    }
}

void Game::handleInput()
{
    if (input::IsKeyPressed(GLFW_KEY_UP))
    {
        rotateCurrentPiece();
    }
    if (input::IsKeyPressed(GLFW_KEY_RIGHT))
    {
        tryMove(mCurrentTetro, {1.0F, 0.0F});
    }
    else if (input::IsKeyPressed(GLFW_KEY_LEFT))
    {
        tryMove(mCurrentTetro, {-1.0F, 0.0F});
    }
}

void Game::updatePieceTransforms(Tetromino& tTetro)
{
    for (std::size_t i{0}; i < tTetro.entities.size(); ++i)
    {
        scene::GetComponent_Transform(tTetro.entities[i])->position = glm::vec3{tTetro.center + tTetro.offsets[i], kStartPos.z};
    }
}

void Game::lockCurrentPiece()
{
    for (std::size_t i{0}; i < mCurrentTetro.entities.size(); ++i)
    {
        auto const cell = toBoardCell(mCurrentTetro.center + mCurrentTetro.offsets[i]);
        auto const row  = static_cast<std::size_t>(cell.y);
        auto const col  = static_cast<std::size_t>(cell.x);

        mBoard[row][col].entity = mCurrentTetro.entities[i];
    }

    checkRowCompletion();
    mCurrentTetro = createPiece();
}

void Game::checkRowCompletion()
{
    size_t writeRow{1U};
    for (size_t readRow{1U}; readRow < kGridH; ++readRow)
    {
        auto&      row  = mBoard[readRow];
        bool const full = std::all_of(row.begin() + 1, row.end() - 1, [](BoardCell const& cell) {
            return cell.entity != ecs::kInvalidEntityHandle;
        });

        if (full)
        {
            for (size_t col{1U}; col < kGridW - 1; ++col)
            {
                scene::RetireEntity(row[col].entity);
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
                    scene::GetComponent_Transform(cell.entity)->position.y -= drop;
                }
                mBoard[writeRow][col] = cell;
                cell                  = {};
            }
        }

        ++writeRow;
    }
}

void Game::restart()
{
    for (std::size_t row{1U}; row < kGridH; ++row)
    {
        for (std::size_t col{1U}; col < kGridW - 1; ++col)
        {
            auto& cell = mBoard[row][col];
            if (cell.entity != ecs::kInvalidEntityHandle)
            {
                scene::RetireEntity(cell.entity);
                cell = {};
            }
        }
    }

    for (auto const& piece : mNextTetro.entities)
    {
        scene::RetireEntity(piece);
    }
    mNextTetro = {};

    mGameOver        = false;
    mAccumulatedTime = 0.0F;

    std::ranges::shuffle(mPieceBag, mGenerator);
    std::iter_swap(mPieceBag.begin(), std::ranges::find(mPieceBag, &CreateHero));
    mNextPiece    = 0U;
    mNextTetro    = CreateTetromino(mPieceBag[mNextPiece++](glm::vec2{kHintStartPos}), kHintStartPos.z);
    mCurrentTetro = createPiece();
}

}// namespace ctc
