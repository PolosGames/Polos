///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_MATERIAL_MATERIAL_LOADER_HPP
#define POLOS_RESOURCE_MATERIAL_MATERIAL_LOADER_HPP

#include "polos/resource/material.hpp"

#include <filesystem>
#include <memory>

namespace polos::resource::material
{
[[nodiscard]] auto LoadMaterial(std::filesystem::path const& tPath) -> std::shared_ptr<Material>;
}// namespace polos::resource::material

#endif// POLOS_RESOURCE_MATERIAL_MATERIAL_LOADER_HPP
