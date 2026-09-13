//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/scene.hpp"

#include "polos/rendering/camera3d.hpp"

#include <glm/gtc/quaternion.hpp>

#include <cassert>
#include <cstdint>
#include <memory>

namespace polos::rendering
{

static constexpr std::int32_t kCamFov = 60;

Scene::Scene()
{
    m_cameras.push_back(std::make_unique<Camera3D>());
    auto& cam = m_cameras.back();

    cam->position = glm::vec3(0.0F, 0.0F, 2.0F);
    cam->rotation = glm::vec3(0.0F, 0.0F, 0.0F);

    // Compute forward vector from pitch and yaw
    glm::quat pitch_quat = glm::angleAxis(glm::radians(cam->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat yaw_quat   = glm::angleAxis(glm::radians(cam->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    // Apply Yaw first, then Pitch
    glm::quat orientation = yaw_quat * pitch_quat;

    cam->target = cam->position + (orientation * glm::vec3(0.0F, 0.0F, -1.0F));
    cam->up     = glm::vec3(0.0F, 1.0F, 0.0F);
    cam->fov    = kCamFov;
    cam->near_z = 0.1F;
    cam->far_z  = 10.0F;
}

Scene::~Scene() = default;

auto Scene::AddObject(RenderObject const& t_object) -> std::size_t
{
    std::size_t const slot = m_objects_current_size++;
    if (slot == m_objects.size())
    {
        m_objects.push_back(t_object);
        m_slot_to_handle.push_back(kInvalidHandle);
    }

    else
    {
        m_objects[slot] = t_object;
    }

    std::size_t handle = kInvalidHandle;
    if (kInvalidHandle != m_free_handle_head)
    {
        handle                   = m_free_handle_head;
        m_free_handle_head       = m_handle_to_slot[handle];
        m_handle_to_slot[handle] = slot;
    }
    else
    {
        handle = m_handle_to_slot.size();
        m_handle_to_slot.push_back(slot);
    }

    m_slot_to_handle[slot] = handle;
    return handle;
}

auto Scene::AddObject(glm::mat4 t_transform, glm::vec4 t_color) -> std::size_t
{ return AddObject(RenderObject{.transform = t_transform, .color = t_color}); }

auto Scene::RemoveObject(std::size_t t_handle) -> void
{
    assert(t_handle < m_handle_to_slot.size() && "Removing a handle that was never allocated!");

    std::size_t const slot      = m_handle_to_slot[t_handle];
    std::size_t const last_slot = --m_objects_current_size;

    if (slot != last_slot)
    {
        m_objects[slot]                = m_objects[last_slot];
        std::size_t const moved_handle = m_slot_to_handle[last_slot];
        m_slot_to_handle[slot]         = moved_handle;
        m_handle_to_slot[moved_handle] = slot;
    }

    m_handle_to_slot[t_handle] = m_free_handle_head;
    m_free_handle_head         = t_handle;
}

auto Scene::GetObjects() const -> std::span<RenderObject const>
{ return {m_objects.data(), m_objects_current_size}; }

auto Scene::GetObject(std::size_t t_handle) -> RenderObject&
{ return m_objects[m_handle_to_slot[t_handle]]; }

auto Scene::GetCamera(std::size_t t_index) -> Camera3D*
{ return m_cameras[t_index].get(); }

}// namespace polos::rendering