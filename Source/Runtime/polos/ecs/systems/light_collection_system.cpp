///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/systems/light_collection_system.hpp"

#include "polos/ecs/components/light_component.hpp"
#include "polos/ecs/components/transform_component.hpp"
#include "polos/ecs/view.hpp"
#include "polos/scene/scene.hpp"

namespace polos::ecs
{

void LightCollectionSystem::Update(scene::Scene* tScene)
{
    if (tScene == nullptr)
    {
        return;
    }

    auto& renderData = tScene->mRenderData;
    auto& lightInfo  = renderData.lightInfo;
    lightInfo        = {};

    for (auto [entity, light] : View<EntityHandle, Read<LightComponent>>{tScene})
    {
        if (lightInfo.lightCount == Settings::kMaxLights)
        {
            continue;
        }
        if (tScene->HasComponent<TransformComponent>(entity))
        {
            auto const& transform                             = *tScene->GetComponent<TransformComponent>(entity);
            renderData.lightInstances[lightInfo.lightCount++] = {
                .position       = transform.position,
                .ambientFactor  = light->ambientFactor,
                .color          = light->color,
                .specularFactor = light->specularFactor,
            };
        }
        else
        {
            lightInfo.color         = light->color;
            lightInfo.ambientFactor = light->ambientFactor;
        }
    }
}

}// namespace polos::ecs
