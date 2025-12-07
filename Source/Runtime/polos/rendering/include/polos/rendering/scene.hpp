//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SCENE_HPP
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SCENE_HPP

#include "polos/rendering/module_macros.hpp"
#include "polos/rendering/render_object.hpp"

#include <glm/glm.hpp>

#include <span>
#include <vector>

namespace polos::rendering
{

class RENDERING_EXPORT Scene
{
public:
    Scene();
    ~Scene();

    Scene(Scene const&)            = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(Scene const&) = delete;
    Scene& operator=(Scene&&)      = delete;

    void                     AddObject(render_object const& t_object);
    void                     AddObject(glm::mat4 const& t_transform, std::shared_ptr<material> const& t_material);
    std::span<render_object> GetObjects();
private:
    glm::mat4 m_view_matrix{0.0F};
    glm::mat4 m_projection_matrix{0.0F};

    std::vector<render_object> m_objects;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SCENE_HPP
