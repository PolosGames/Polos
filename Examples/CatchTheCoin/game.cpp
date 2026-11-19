///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "game.hpp"

#include "helpers.hpp"
#include "polos/rendering/debug_draw.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <polos/ecs/scene.hpp>

namespace ctc
{

using namespace polos;

void Game::Setup(polos::ecs::Scene* tScene)
{
    mScene = tScene;

    CreateEntityFromModel("model.scene"_sid, tScene);
    CreateQuad(
        tScene,
        CreateMaterialFromTexture(
            "material.Ground", {.img = "texture.yd0lacrs_1K_Basecolor"_sid, .textureDetails = {}}),
        ecs::TransformComponent{.rotation = {-90.0F, 0.0F, 0.0F}});
    mCharacterCam = CreateCamera(tScene, {});
}

void Game::Update(float const tDt)
{}

void Game::Render(float const tDt)
{
    rendering::DebugDraw::Sphere(mScene->GetComponent<ecs::TransformComponent>(mCharacterCam)->position, .3334F);

    renderEditor(tDt);
}

void Game::renderEditor(float tDt)
{
    // clang-format off
    ImGui::SetNextWindowSize({250.0F, 380.0F});
    if (ImGui::Begin("Editor"))
    {
        ImGui::Checkbox("Show Demo", &mShowDemo);
        ImGui::BeginChild("Hierarchy", ImVec2(240.0F, 330.0F), ImGuiChildFlags_Borders, ImGuiWindowFlags_MenuBar);
            ImGui::BeginMenuBar();
                ImGui::BeginMenu("Hierarchy");
                ImGui::EndMenu();
            ImGui::EndMenuBar();
        ImGui::EndChild();
    }
    ImGui::End();
    // clang-format on

    if (mShowDemo)
    {
        ImGui::ShowDemoWindow(&mShowDemo);
    }
}

}// namespace ctc
