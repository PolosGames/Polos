//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/scene.hpp"

namespace polos::rendering
{

Scene::Scene()  = default;
Scene::~Scene() = default;

std::size_t Scene::AddObject(RenderObject const& t_object)
{
    m_objects.push_back(t_object);
    return m_objects.size() - 1;
}

std::size_t Scene::AddObject(glm::mat4 const& t_transform, std::shared_ptr<Material> const& t_material)
{
    m_objects.emplace_back(t_transform, t_material);
    return m_objects.size() - 1;
}

std::span<RenderObject> Scene::GetObjects()
{
    return m_objects;
}

RenderObject& Scene::GetObject(std::size_t t_index)
{
    return m_objects[t_index];
}

}// namespace polos::rendering