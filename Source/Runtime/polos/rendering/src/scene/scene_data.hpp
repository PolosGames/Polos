///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_SCENE_SCENE_DATA_HPP
#define POLOS_RENDERING_SRC_SCENE_SCENE_DATA_HPP

#include "polos/rendering/scene/render_object.hpp"

#include <span>

namespace polos::rendering
{

struct Camera3D;

struct SceneData
{
    Camera3D*                     camera;
    std::span<RenderObject const> objects;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_SCENE_SCENE_DATA_HPP
