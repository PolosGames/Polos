///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_SCENE_CAMERA_UNIFORMS_HPP
#define POLOS_RENDERING_SRC_SCENE_CAMERA_UNIFORMS_HPP

#include "polos/rendering/scene/camera3d.hpp"
#include "resources/uniform_buffer_object.hpp"

#include <cstdint>

namespace polos::rendering
{

auto GetCameraUbo(Camera3D const* tCam, std::uint32_t tScWidth, std::uint32_t tScHeight) -> UniformBufferObject;

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_SCENE_CAMERA_UNIFORMS_HPP
