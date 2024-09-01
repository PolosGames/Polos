#pragma once

#include "polos/graphics/quad_vertex.h"

namespace polos::resource
{
    // Single texture for gltf, 2 textures with other 3d file types
    using MetallicRoughnessResourceHandle = std::variant<ResourceHandle, std::pair<ResourceHandle, ResourceHandle>>;

    struct pbr_material
    {
        ResourceHandle                  albedoMap;
        ResourceHandle                  normalMap;
        int32                           metallicValue;
        int32                           roughnessValue;
        MetallicRoughnessResourceHandle metallicRoughnessMap;
    };

    struct mesh_info
    {
        std::pair<std::size_t, std::size_t> verticesSlice;
        std::pair<std::size_t, std::size_t> indicesSlice;
        std::size_t materialIndex;
    };

    struct node_info
    {
        std::pair<std::size_t, std::size_t> childrenNodes;
        std::pair<std::size_t, std::size_t> meshesSlice;
    };

    struct alignas(64) model
    {
        DArray<quad_vertex>       vertices;
        DArray<uint32>       indices;
        DArray<pbr_material> materials;

        DArray<mesh_info> meshes;
        DArray<node_info> nodes;
    };
} // namespace polos::resource
