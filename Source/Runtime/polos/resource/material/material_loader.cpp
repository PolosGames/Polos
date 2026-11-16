///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/material/material_loader.hpp"

namespace polos::resource::material
{
auto LoadMaterial(std::filesystem::path const& tPath) -> std::shared_ptr<Material>
{
    auto* material = new Material{};
    return {material, [](Material* tPtr) {
                delete tPtr;
            }};
}
}// namespace polos::resource::material
