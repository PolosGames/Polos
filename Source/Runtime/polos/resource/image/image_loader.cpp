///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/image/image_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/resource/image.hpp"

#include <stb_image.h>

#include <limits>
#include <utility>

namespace polos::resource::image
{
namespace
{
constexpr std::int32_t kOutputComponents{4};

auto MakeImage(std::uint8_t* tPixels, std::int32_t tWidth, std::int32_t tHeight) -> std::shared_ptr<Image>
{
    auto const imgSize = static_cast<std::size_t>(tWidth) * static_cast<std::size_t>(tHeight) * kOutputComponents;
    return std::shared_ptr<Image>(
        new Image{
            .width      = tWidth,
            .height     = tHeight,
            .components = kOutputComponents,
            .data       = std::span<std::uint8_t>(tPixels, imgSize),
        },
        [](Image* tPtr) -> void {
            stbi_image_free(tPtr->data.data());
            delete tPtr;
        });
}
}// namespace

auto LoadImage(std::filesystem::path const& tPath) -> std::shared_ptr<Image>
{
    std::int32_t width{0};
    std::int32_t height{0};
    std::int32_t sourceComponents{0};
    auto const pathUtf8 = tPath.u8string();
    auto const* path = reinterpret_cast<char const*>(pathUtf8.c_str());
    if (1U != stbi_info(path, &width, &height, &sourceComponents))
    {
        LogError("Could not read image information for '{}': {}", path, stbi_failure_reason());
        return nullptr;
    }

    stbi_set_flip_vertically_on_load_thread(1);
    std::uint8_t* pixels = stbi_load(path, &width, &height, &sourceComponents, kOutputComponents);
    if (nullptr == pixels)
    {
        LogError("Could not decode image '{}': {}", path, stbi_failure_reason());
        return nullptr;
    }

    return MakeImage(pixels, width, height);
}

auto LoadImage(std::span<std::byte const> tData) -> std::shared_ptr<Image>
{
    if (tData.empty() || std::cmp_greater(tData.size(), std::numeric_limits<int>::max()))
    {
        LogError("Encoded image data is empty or exceeds the decoder's size limit.");
        return nullptr;
    }

    std::int32_t width{0};
    std::int32_t height{0};
    std::int32_t sourceComponents{0};
    stbi_set_flip_vertically_on_load_thread(1);
    auto* pixels = stbi_load_from_memory(
        reinterpret_cast<stbi_uc const*>(tData.data()),
        static_cast<int>(tData.size()),
        &width,
        &height,
        &sourceComponents,
        kOutputComponents);
    if (pixels == nullptr)
    {
        LogError("Could not decode image data: {}", stbi_failure_reason());
        return nullptr;
    }

    return MakeImage(pixels, width, height);
}

}// namespace polos::resource::image
