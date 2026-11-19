///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef SANDBOX_HELPERS_HPP
#define SANDBOX_HELPERS_HPP

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/components/transform_component.hpp"

#include <glm/glm.hpp>
#include <polos/ecs/components/light_component.hpp>
#include <polos/ecs/entity.hpp>
#include <polos/rendering/cache/texture_cache.hpp>
#include <polos/utils/string_id.hpp>

namespace polos::scene
{
class Scene;
}// namespace polos::scene

namespace ctc
{
auto CreateEntityFromModel(
    polos::scene::Scene*                  tScene,
    polos::utils::string_id               tModel,
    polos::ecs::TransformComponent const& tTransform = {},
    std::string_view                      tName      = "New Entity") -> polos::ecs::EntityHandle;

auto CreateLightSource(polos::scene::Scene* tScene, polos::ecs::LightComponent const& tLight = {}, std::string_view tName = "New Light")
    -> polos::ecs::EntityHandle;

auto CreateQuad(
    polos::scene::Scene*                  tScene,
    polos::rendering::MaterialHandle      tMaterial,
    polos::ecs::TransformComponent const& tTransform = {},
    std::string_view                      tName      = "New Quad") -> polos::ecs::EntityHandle;

auto CreateMaterialFromTexture(
    std::string_view                        tTexture,
    std::string_view                        tName           = {},
    polos::rendering::SamplerDetails const& tSamplerDetails = {},
    float                                   tTilingFactor   = 1.0F) -> polos::rendering::MaterialHandle;

auto CreateCamera(
    polos::scene::Scene*                  tScene,
    polos::ecs::TransformComponent const& tTransform = {},
    polos::ecs::CameraType                tType      = polos::ecs::CameraType::kPerspective,
    std::string_view                      tName      = "New Camera",
    polos::ecs::EntityHandle              tParent    = polos::ecs::EntityHandle{.index = 0U, .version = 0U}) -> polos::ecs::EntityHandle;

auto CreateCube(
    polos::scene::Scene*                  tScene,
    polos::ecs::TransformComponent const& tTransform = {},
    polos::rendering::MaterialHandle      tMaterial  = polos::rendering::kDefaultMaterial,
    std::string_view                      tName      = "New Cube",
    polos::ecs::EntityHandle              tParent    = polos::ecs::EntityHandle{.index = 0U, .version = 0U}) -> polos::ecs::EntityHandle;

}// namespace ctc

#endif// SANDBOX_HELPERS_HPP
