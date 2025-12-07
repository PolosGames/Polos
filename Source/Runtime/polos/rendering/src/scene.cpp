//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/scene.hpp"

namespace polos::rendering
{

Scene::Scene()  = default;
Scene::~Scene() = default;

void Scene::AddObject(render_object const& t_object)
{
    m_objects.push_back(t_object);
}

void Scene::AddObject(glm::mat4 const& t_transform, std::shared_ptr<material> const& t_material)
{
    m_objects.emplace_back(t_transform, t_material);
}

std::span<render_object> Scene::GetObjects()
{
    return m_objects;
}

}// namespace polos::rendering