///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_INCLUDE_POLOS_RESOURCE_IMAGE_LOADER_HPP
#define POLOS_RESOURCE_INCLUDE_POLOS_RESOURCE_IMAGE_LOADER_HPP

#include "polos/polos_api.hpp"
#include "polos/resource/image.hpp"
#include "polos/resource/loader.hpp"

#include <string_view>

namespace polos::resource
{
template<>
class POLOS_API Loader<Image>
{
public:
    explicit Loader(std::string_view tPath);

    Image resource;
};
}// namespace polos::resource

#endif// POLOS_RESOURCE_INCLUDE_POLOS_RESOURCE_IMAGE_LOADER_HPP
