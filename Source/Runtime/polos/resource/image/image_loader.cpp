///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/image/image_loader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/resource/image.hpp"

#include <stb_image.h>

namespace polos::resource::image
{

auto LoadImage(std::filesystem::path const& tPath) -> std::shared_ptr<Image>
{
    constexpr std::int32_t kOutputComponents{4};
    std::int32_t           width{0};
    std::int32_t           height{0};
    std::int32_t           sourceComponents{0};
    if (1U != stbi_info(tPath.c_str(), &width, &height, &sourceComponents))
    {
        LogError("Could not read image information for '{}': {}", tPath.string(), stbi_failure_reason());
        return nullptr;
    }

    stbi_set_flip_vertically_on_load_thread(1);
    std::uint8_t* pixels = stbi_load(tPath.c_str(), &width, &height, &sourceComponents, kOutputComponents);
    if (nullptr == pixels)
    {
        LogError("Could not decode image '{}': {}", tPath.string(), stbi_failure_reason());
        return nullptr;
    }

    auto const imgSize = static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height) * kOutputComponents;

    return std::shared_ptr<Image>(
        new Image{
            .width      = width,
            .height     = height,
            .components = kOutputComponents,
            .data       = std::span<std::uint8_t>(pixels, imgSize),
        },
        [](Image* tPtr) -> void {
            stbi_image_free(tPtr->data.data());
            delete tPtr;
        });
}

}// namespace polos::resource::image
