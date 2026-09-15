//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_SCENE_SCENE_HPP
#define POLOS_RENDERING_SCENE_SCENE_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/scene/camera3d.hpp"
#include "polos/rendering/scene/render_object.hpp"

#include <glm/glm.hpp>

#include <span>
#include <vector>

namespace polos::rendering
{

class POLOS_API Scene
{
public:
    Scene();
    ~Scene();

    Scene(Scene const&)            = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene const&) = delete;
    Scene& operator=(Scene&&)      = delete;

    /// @brief Adds object, returns a handle stable until RemoveObject is called with it.
    [[nodiscard]] auto AddObject(RenderObject const& tObject) -> std::size_t;

    /// @brief Adds object, returns a handle stable until RemoveObject is called with it.
    [[nodiscard]] auto AddObject(glm::mat4 tTransform, glm::vec4 tColor) -> std::size_t;

    /// @brief Frees tHandle's slot for reuse by a later AddObject call.
    auto RemoveObject(std::size_t tHandle) -> void;

    /// @brief Returns all objects in scene
    [[nodiscard]] auto GetObjects() const -> std::span<RenderObject const>;

    /// @brief Returns single object in scene at tHandle
    [[nodiscard]] auto GetObject(std::size_t tHandle) -> RenderObject&;

    /// @brief Returns the camera in scene at tIndex
    [[nodiscard]] auto GetCamera(std::size_t tIndex) -> Camera3D*;
private:
    static constexpr std::size_t kInvalidHandle = static_cast<std::size_t>(-1);

    std::vector<std::unique_ptr<Camera3D>> mCameras;

    std::vector<RenderObject> mObjects;
    std::vector<std::size_t>  mSlotToHandle;
    std::vector<std::size_t>  mHandleToSlot;
    std::size_t               mFreeHandleHead{kInvalidHandle};
    std::size_t               mObjectsCurrentSize{0U};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SCENE_SCENE_HPP
