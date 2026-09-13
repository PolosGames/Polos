///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_VIEW_HPP
#define POLOS_RENDERING_RENDER_VIEW_HPP

#include "polos/polos_api.hpp"

#include <span>

namespace polos::rendering
{

struct Camera3D;
struct RenderObject;

struct RenderView
{
    Camera3D const*               camera;
    std::span<RenderObject const> objects;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_VIEW_HPP
