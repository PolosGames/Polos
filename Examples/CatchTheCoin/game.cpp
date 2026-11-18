///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "game.hpp"

#include "helpers.hpp"

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
}

void Game::Update(float const tDt)
{}

void Game::Render(float const tDt)
{
    if (ImGui::Begin("Editor"))
    {
        ImGui::Checkbox("Show Demo", &mShowDemo);
        if (auto const* transform = mScene->GetComponent<ecs::TransformComponent>(mScene->activeCamera);
            nullptr != transform)
        {
            glm::vec3 pos = transform->position;
            ImGui::DragFloat3("", glm::value_ptr(pos));
        }
    }
    ImGui::End();

    ImGui::ShowDemoWindow(&mShowDemo);
}

}// namespace ctc
