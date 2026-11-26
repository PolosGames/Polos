///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_MATERIAL_MATERIAL_LOADER_HPP
#define POLOS_RESOURCE_MATERIAL_MATERIAL_LOADER_HPP

#include "polos/rendering/resources/material.hpp"

#include <filesystem>
#include <optional>

namespace polos::resource::material
{
[[nodiscard]] auto LoadMaterial(std::filesystem::path const& tPath) -> std::optional<rendering::Material>;
}// namespace polos::resource::material

#endif// POLOS_RESOURCE_MATERIAL_MATERIAL_LOADER_HPP
