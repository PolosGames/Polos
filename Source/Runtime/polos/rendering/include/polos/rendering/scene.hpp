//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_SCENE_HPP
#define POLOS_RENDERING_SCENE_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/camera3d.hpp"
#include "polos/rendering/render_object.hpp"

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
    [[nodiscard]] auto AddObject(RenderObject const& t_object) -> std::size_t;

    /// @brief Adds object, returns a handle stable until RemoveObject is called with it.
    [[nodiscard]] auto AddObject(glm::mat4 t_transform, glm::vec4 t_color) -> std::size_t;

    /// @brief Frees t_handle's slot for reuse by a later AddObject call.
    auto RemoveObject(std::size_t t_handle) -> void;

    /// @brief Returns all objects in scene
    [[nodiscard]] auto GetObjects() const -> std::span<RenderObject const>;

    /// @brief Returns single object in scene at t_handle
    [[nodiscard]] auto GetObject(std::size_t t_handle) -> RenderObject&;

    /// @brief Returns the camera in scene at t_index
    [[nodiscard]] auto GetCamera(std::size_t t_index) -> Camera3D*;
private:
    static constexpr std::size_t kInvalidHandle = static_cast<std::size_t>(-1);

    std::vector<std::unique_ptr<Camera3D>> m_cameras;

    std::vector<RenderObject> m_objects;
    std::vector<std::size_t>  m_slot_to_handle;
    std::vector<std::size_t>  m_handle_to_slot;
    std::size_t               m_free_handle_head{kInvalidHandle};
    std::size_t               m_objects_current_size{0U};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SCENE_HPP
