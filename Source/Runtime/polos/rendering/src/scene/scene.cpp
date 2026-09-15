//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/scene/scene.hpp"

#include "polos/rendering/scene/camera3d.hpp"

#include <glm/gtc/quaternion.hpp>

#include <cassert>
#include <cstdint>
#include <memory>

namespace polos::rendering
{

static constexpr std::int32_t kCamFov = 60;

Scene::Scene()
{
    mCameras.push_back(std::make_unique<Camera3D>());
    auto& cam = mCameras.back();

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
    cam->nearZ  = 0.1F;
    cam->farZ   = 10.0F;
}

Scene::~Scene() = default;

auto Scene::AddObject(RenderObject const& tObject) -> std::size_t
{
    std::size_t const slot = mObjectsCurrentSize++;
    if (slot == mObjects.size())
    {
        mObjects.push_back(tObject);
        mSlotToHandle.push_back(kInvalidHandle);
    }

    else
    {
        mObjects[slot] = tObject;
    }

    std::size_t handle = kInvalidHandle;
    if (kInvalidHandle != mFreeHandleHead)
    {
        handle                = mFreeHandleHead;
        mFreeHandleHead       = mHandleToSlot[handle];
        mHandleToSlot[handle] = slot;
    }
    else
    {
        handle = mHandleToSlot.size();
        mHandleToSlot.push_back(slot);
    }

    mSlotToHandle[slot] = handle;
    return handle;
}

auto Scene::AddObject(glm::mat4 tTransform, glm::vec4 tColor) -> std::size_t
{ return AddObject(RenderObject{.transform = tTransform, .color = tColor}); }

auto Scene::RemoveObject(std::size_t tHandle) -> void
{
    assert(tHandle < mHandleToSlot.size() && "Removing a handle that was never allocated!");

    std::size_t const slot      = mHandleToSlot[tHandle];
    std::size_t const last_slot = --mObjectsCurrentSize;

    if (slot != last_slot)
    {
        mObjects[slot]                 = mObjects[last_slot];
        std::size_t const moved_handle = mSlotToHandle[last_slot];
        mSlotToHandle[slot]            = moved_handle;
        mHandleToSlot[moved_handle]    = slot;
    }

    mHandleToSlot[tHandle] = mFreeHandleHead;
    mFreeHandleHead        = tHandle;
}

auto Scene::GetObjects() const -> std::span<RenderObject const>
{ return {mObjects.data(), mObjectsCurrentSize}; }

auto Scene::GetObject(std::size_t tHandle) -> RenderObject&
{ return mObjects[mHandleToSlot[tHandle]]; }

auto Scene::GetCamera(std::size_t tIndex) -> Camera3D*
{ return mCameras[tIndex].get(); }

}// namespace polos::rendering