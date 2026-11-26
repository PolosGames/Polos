///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_MODEL_HPP
#define POLOS_RENDERING_RESOURCES_MODEL_HPP

#include "polos/rendering/resources/mesh.hpp"

#include <glm/gtc/quaternion.hpp>

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace polos::rendering
{

struct Node
{
    std::string                name;
    glm::vec3                  position{0.0F};
    glm::quat                  rotation{1.0F, 0.0F, 0.0F, 0.0F};
    float                      scale{1.0F};
    std::optional<std::size_t> meshIndex;
    std::vector<std::size_t>   children;
};

struct Model
{
    std::string              name;
    std::vector<Mesh>        meshes;
    std::vector<Node>        nodes;
    std::vector<std::size_t> rootNodes;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_MODEL_HPP
