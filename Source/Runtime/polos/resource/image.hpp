///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_IMAGE_HPP
#define POLOS_RESOURCE_IMAGE_HPP

#include <cstdint>
#include <span>

namespace polos::resource::image
{
struct Image
{
    std::int32_t            width{0};
    std::int32_t            height{0};
    std::int32_t            components{0};
    std::span<std::uint8_t> data;
};
}// namespace polos::resource::image

#endif// POLOS_RESOURCE_IMAGE_HPP
