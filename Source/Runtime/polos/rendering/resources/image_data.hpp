///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RESOURCES_IMAGE_DATA_HPP
#define POLOS_RENDERING_RESOURCES_IMAGE_DATA_HPP

#include <cstdint>
#include <span>

namespace polos::rendering
{

struct ImageData
{
    std::int32_t            width{0};
    std::int32_t            height{0};
    std::int32_t            components{0};
    std::span<std::uint8_t> data;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RESOURCES_IMAGE_DATA_HPP
