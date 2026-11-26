///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_IMAGE_LOADER_HPP
#define POLOS_RESOURCE_IMAGE_LOADER_HPP


#include <cstddef>
#include <filesystem>
#include <memory>
#include <span>

namespace polos::rendering
{
struct ImageData;
}

namespace polos::resource::image
{

/// @return An RGBA8 image with four components, or nullptr if reading or decoding fails.
[[nodiscard]] auto LoadImage(std::filesystem::path const& tPath) -> std::shared_ptr<rendering::ImageData>;
/// Decode encoded image bytes into an owned RGBA8 image, or return nullptr on failure.
[[nodiscard]] auto LoadImage(std::span<std::byte const> tData) -> std::shared_ptr<rendering::ImageData>;
}// namespace polos::resource::image

#endif// POLOS_RESOURCE_IMAGE_LOADER_HPP
