///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_IMAGE_LOADER_HPP
#define POLOS_RESOURCE_IMAGE_LOADER_HPP


#include <filesystem>
#include <memory>

namespace polos::resource::image
{
struct Image;

/// @return An RGBA8 image with four components, or nullptr if reading or decoding fails.
[[nodiscard]] auto LoadImage(std::filesystem::path const& tPath) -> std::shared_ptr<Image>;
}// namespace polos::resource::image

#endif// POLOS_RESOURCE_IMAGE_LOADER_HPP
