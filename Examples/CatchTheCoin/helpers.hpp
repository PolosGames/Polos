///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef SANDBOX_HELPERS_HPP
#define SANDBOX_HELPERS_HPP

#include "polos/ecs/components/transform_component.hpp"

#include <glm/glm.hpp>
#include <polos/ecs/components/light_component.hpp>
#include <polos/ecs/entity.hpp>
#include <polos/rendering/cache/texture_cache.hpp>
#include <polos/utils/string_id.hpp>

namespace polos::ecs
{
class Scene;
}// namespace polos::ecs

namespace ctc
{
auto CreateEntityFromModel(polos::utils::string_id tModel, polos::ecs::Scene* tScene) -> polos::ecs::EntityHandle;

auto CreateLightSource(polos::ecs::Scene* tScene, polos::ecs::LightComponent const& tLight = {})
    -> polos::ecs::EntityHandle;

void CreateQuad(
    polos::ecs::Scene*                    tScene,
    polos::rendering::MaterialHandle      tMaterial,
    polos::ecs::TransformComponent const& tTransform = {});

auto CreateMaterialFromTexture(std::string_view tName, polos::resource::material::MaterialTexture const& tTexture)
    -> polos::rendering::MaterialHandle;

auto CreateCamera(polos::ecs::Scene* tScene, polos::ecs::TransformComponent const& tTransform)
    -> polos::ecs::EntityHandle;

}// namespace ctc

#endif// SANDBOX_HELPERS_HPP
