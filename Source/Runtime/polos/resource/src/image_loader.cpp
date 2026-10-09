///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/image_loader.hpp"

#include "polos/logging/log_macros.hpp"

namespace polos::resource
{

Loader<Image>::Loader(std::string_view tPath)
{ LogInfo("Path {}", tPath); }

}// namespace polos::resource
