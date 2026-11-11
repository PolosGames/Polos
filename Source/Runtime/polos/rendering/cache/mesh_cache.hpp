///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CACHE_MESH_CACHE_HPP
#define POLOS_RENDERING_CACHE_MESH_CACHE_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/settings.hpp"
#include "polos/resource/model.hpp"
#include "polos/utils/error_code.hpp"
#include "polos/utils/pair.hpp"

#include <cstdint>
#include <map>
#include <memory>

namespace polos::rendering
{

using MeshHandle = utils::string_id;

inline constexpr MeshHandle kPolosDefaultCube = "mesh.PolosDefaultCube.part.0"_sid;

struct Mesh
{
    int64_t vertexCount{0};
    int64_t indexCount{0};
};

class POLOS_API MeshCache
{
public:
    struct CreationDetails
    {
    };

    MeshCache();
    ~MeshCache();

    auto Create(CreationDetails const& tDetails) -> Result<void>;
    auto Destroy() -> void;

    [[nodiscard]] static auto AddMesh(resource::model::MeshPart& tMesh) -> MeshHandle;

    /// @brief Check if we have a mesh with the handle
    [[nodiscard]] static auto HasMesh(MeshHandle tHandle) -> bool;

    [[nodiscard]] static auto GetMeshIndexByHandle(MeshHandle tHandle) -> uint32_t;

    [[nodiscard]] static auto GetVertexBuffer() -> VkBuffer;
    [[nodiscard]] static auto GetIndexBuffer() -> VkBuffer;
    [[nodiscard]] static auto GetMeshBoundsBuffer() -> VkBuffer;
    [[nodiscard]] static auto GetMeshCount() -> uint32_t;
    [[nodiscard]] static auto GetIndirectCommandBufferByFrameSlot(uint32_t tFrameSlot) -> VkBuffer;

    static auto IncreaseMeshUse(MeshHandle tHandle) -> void;
    static auto DecreaseMeshUse(MeshHandle tHandle) -> void;
    static auto IncreaseMeshUseByIndex(uint32_t tMeshIndex) -> void;
    static auto DecreaseMeshUseByIndex(uint32_t tMeshIndex) -> void;
    static auto FlushMeshUses(uint32_t tFrameSlot) -> void;
private:
    static MeshCache* sInstance;

    struct Entry
    {
        utils::string_id name{0};
        VkDeviceSize     vertexOffset{0U};
        VkDeviceSize     indexOffset{0U};
        uint32_t         vertexCount{0};
        uint32_t         indexCount{0};
        uint32_t         meshIndex{0U};
    };

    std::unique_ptr<Buffer> mVertexBuffer;
    std::unique_ptr<Buffer> mIndexBuffer;

    std::vector<uint32_t>                                             mMeshInstances;
    std::vector<VkDrawIndexedIndirectCommand>                         mMeshInfos;
    std::vector<glm::vec4>                                            mMeshBounds;
    std::unique_ptr<Buffer>                                           mMeshBoundsBuffer;
    std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight> mMeshInfoBuffer;

    // How many new uses added/removed of a specific mesh at a index
    std::vector<utils::Pair<uint32_t, int32_t>> mMeshUses;

    VkDeviceSize mVertexHead{0U};
    VkDeviceSize mIndexHead{0U};

    std::map<MeshHandle, Entry> mMeshes;
    uint32_t                    mMeshCount{0U};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_CACHE_MESH_CACHE_HPP
