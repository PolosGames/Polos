///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef SANDBOX_HELPERS_HPP
#define SANDBOX_HELPERS_HPP

#include <glm/glm.hpp>
#include <polos/ecs/components/light_component.hpp>
#include <polos/ecs/entity.hpp>
#include <polos/rendering/cache/texture_cache.hpp>
#include <polos/utils/string_id.hpp>

namespace polos::ecs
{
class Scene;
}// namespace polos::ecs

namespace sandbox
{
auto CreateEntityFromModel(polos::utils::string_id tModel, polos::ecs::Scene* tScene) -> polos::ecs::EntityHandle;
auto CreateLightSource(polos::ecs::Scene* tScene, polos::ecs::LightComponent const& tLight = {})
    -> polos::ecs::EntityHandle;
void CreateQuad(
    polos::ecs::Scene*               tScene,
    glm::vec3                        tPosition,
    glm::vec3                        tRotation,
    float                            tScale,
    polos::rendering::MaterialHandle tMaterial);
auto CreateMaterialFromTexture(std::string_view tName, polos::rendering::TextureHandle tHandle)
    -> polos::rendering::MaterialHandle;

}// namespace sandbox

#endif// SANDBOX_HELPERS_HPP
