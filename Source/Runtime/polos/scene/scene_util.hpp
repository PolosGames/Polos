///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_SCENE_SCENE_UTIL_HPP
#define POLOS_SCENE_SCENE_UTIL_HPP

#include "polos/ecs/components/camera_component.hpp"
#include "polos/ecs/entity.hpp"
#include "polos/polos_api.hpp"
#include "polos/rendering/common.hpp"
#include "polos/resource/common.hpp"

#include <glm/glm.hpp>

#include <string_view>

namespace polos::ecs
{
struct CameraComponent;
struct InfoComponent;
struct LightComponent;
struct MeshRendererComponent;
struct TransformComponent;
}// namespace polos::ecs

/// Operations for current scene.
namespace polos::scene
{

auto POLOS_API NewEntity(ecs::EntityHandle tParent = ecs::EntityHandle{.index = 0U, .version = 0U}, utils::string_id tName = "New Entity"_sid)
    -> ecs::EntityHandle;
auto POLOS_API RetireEntity(ecs::EntityHandle tParent) -> bool;

struct QuadDetails
{
    std::string_view      name{"New Quad"};
    glm::vec2             dimensions{1.0F, 1.0F};
    glm::vec3             position{0.0F};
    glm::vec3             rotation{0.0F};
    float                 scale{1.0F};
    rendering::ResourceId material{rendering::kDefaultMaterial};
    glm::vec2             tilingFactor{1.0F};
};
auto POLOS_API Create_TexturedQuad(QuadDetails const& tDetails = {}, ecs::EntityHandle tParent = ecs::EntityHandle{.index = 0U, .version = 0U})
    -> ecs::EntityHandle;

struct CuboidDetails
{
    std::string_view      name{"New Cuboid"};
    glm::vec3             dimensions{1.0F, 1.0F, 1.0F};
    glm::vec3             position{0.0F};
    glm::vec3             rotation{0.0F};
    float                 scale{1.0F};
    rendering::ResourceId material{rendering::kDefaultMaterial};
};
auto POLOS_API Create_TexturedCuboid(CuboidDetails const& tDetails = {}, ecs::EntityHandle tParent = ecs::EntityHandle{.index = 0U, .version = 0U})
    -> ecs::EntityHandle;

struct LightDetails
{
    std::string_view name{"New Light Source"};
    glm::vec3        position{0.0F};
    glm::vec3        rotation{0.0F};
    float            scale{1.0F};
    glm::vec3        color{1.0F};
    float            ambientFactor{1.0F};
    float            specularFactor{1.0F};
};
auto POLOS_API Create_LightSource(LightDetails const& tDetails = {}, ecs::EntityHandle tParent = ecs::EntityHandle{.index = 0U, .version = 0U})
    -> ecs::EntityHandle;

struct ModelDetails
{
    std::string_view     name;
    glm::vec3            position{0.0F};
    glm::vec3            rotation{0.0F};
    float                scale{1.0F};
    resource::ResourceId modelID;
};
auto POLOS_API Create_Model(ModelDetails const& tDetails = {}, ecs::EntityHandle tParent = ecs::EntityHandle{.index = 0U, .version = 0U}) -> ecs::EntityHandle;

struct CameraDetails
{
    std::string_view name;
    glm::vec3        position{0.0F};
    glm::vec3        rotation{0.0F};
    float            scale{1.0F};
    ecs::CameraType  cameraType{ecs::CameraType::kPerspective};
    float            nearZ{0.1F};
    float            farZ{1000.0F};
    float            fov{60.0F}; // perspective
    float            size{10.0F};// ortho
};
auto POLOS_API Create_Camera(CameraDetails const& tDetails = {}, ecs::EntityHandle tParent = ecs::EntityHandle{.index = 0U, .version = 0U})
    -> ecs::EntityHandle;

auto POLOS_API AddComponent_Camera(ecs::EntityHandle tEntity) -> ecs::CameraComponent*;
auto POLOS_API GetComponent_Camera(ecs::EntityHandle tEntity) -> ecs::CameraComponent*;
auto POLOS_API GetConstComponent_Camera(ecs::EntityHandle tEntity) -> ecs::CameraComponent const*;

auto POLOS_API AddComponent_MeshRenderer(ecs::EntityHandle tEntity) -> ecs::MeshRendererComponent*;
auto POLOS_API GetComponent_MeshRenderer(ecs::EntityHandle tEntity) -> ecs::MeshRendererComponent*;
auto POLOS_API GetConstComponent_MeshRenderer(ecs::EntityHandle tEntity) -> ecs::MeshRendererComponent const*;

auto POLOS_API AddComponent_Transform(ecs::EntityHandle tEntity) -> ecs::TransformComponent*;
auto POLOS_API GetComponent_Transform(ecs::EntityHandle tEntity) -> ecs::TransformComponent*;
auto POLOS_API GetConstComponent_Transform(ecs::EntityHandle tEntity) -> ecs::TransformComponent const*;

auto POLOS_API GetComponent_Info(ecs::EntityHandle tEntity) -> ecs::InfoComponent*;
auto POLOS_API GetConstComponent_Info(ecs::EntityHandle tEntity) -> ecs::InfoComponent const*;

auto POLOS_API AddComponent_Light(ecs::EntityHandle tEntity) -> ecs::LightComponent*;
auto POLOS_API GetComponent_Light(ecs::EntityHandle tEntity) -> ecs::LightComponent*;
auto POLOS_API GetConstComponent_Light(ecs::EntityHandle tEntity) -> ecs::LightComponent const*;

auto POLOS_API EntityAtIndex(ecs::EntityIndex tIndex) -> ecs::EntityHandle;
auto POLOS_API EntityDataAtIndex(ecs::EntityIndex tIndex) -> ecs::Entity const*;
}// namespace polos::scene

#endif// POLOS_SCENE_SCENE_UTIL_HPP
