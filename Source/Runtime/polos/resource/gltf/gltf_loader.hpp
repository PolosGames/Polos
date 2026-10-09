///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_GLTF_LOADER_HPP
#define POLOS_RESOURCE_GLTF_LOADER_HPP


#include <filesystem>
#include <memory>

namespace polos::resource::model
{
struct Model;

/// @return The loaded model, or nullptr if parsing or validation fails.
[[nodiscard]] auto LoadGltf(std::filesystem::path const& tPath) -> std::unique_ptr<Model>;
}// namespace polos::resource::model

#endif// POLOS_RESOURCE_GLTF_LOADER_HPP
