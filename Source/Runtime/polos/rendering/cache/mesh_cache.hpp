///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CACHE_MESH_CACHE_HPP
#define POLOS_RENDERING_CACHE_MESH_CACHE_HPP

#include "polos/rendering/common.hpp"
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

struct Mesh
{
    int64_t vertexCount{0};
    int64_t indexCount{0};
};

class POLOS_API MeshCache
{
public:
    [[nodiscard]] static auto Get() -> MeshCache&;

    struct CreationDetails
    {
    };

    MeshCache();
    ~MeshCache();

    auto Create(CreationDetails const& tDetails) -> Result<void>;
    auto Destroy() -> void;

    [[nodiscard]] auto AddMesh(resource::model::MeshPart& tMesh) -> MeshHandle;

    /// @brief Check if we have a mesh with the handle
    [[nodiscard]] auto HasMesh(MeshHandle tHandle) -> bool;

    [[nodiscard]] auto GetMeshIndexByHandle(MeshHandle tHandle) -> MeshIndex;

    [[nodiscard]] auto GetVertexBuffer() -> VkBuffer;
    [[nodiscard]] auto GetIndexBuffer() -> VkBuffer;
    [[nodiscard]] auto GetMeshBoundsBuffer() -> VkBuffer;
    [[nodiscard]] auto GetMeshBoundsBufferSize() const -> VkDeviceSize;
    [[nodiscard]] auto GetMeshCount() -> uint32_t;
    [[nodiscard]] auto GetIndirectCommandBufferByFrameSlot(uint32_t tFrameSlot) -> VkBuffer;

    auto IncreaseMeshUse(MeshHandle tHandle) -> void;
    auto DecreaseMeshUse(MeshHandle tHandle) -> void;
    auto IncreaseMeshUseByIndex(MeshIndex tMeshIndex) -> void;
    auto DecreaseMeshUseByIndex(MeshIndex tMeshIndex) -> void;
    auto FlushMeshUses(uint32_t tFrameSlot) -> void;
private:
    static MeshCache* sInstance;

    struct Entry
    {
        utils::string_id name{0};
        VkDeviceSize     vertexOffset{0U};
        VkDeviceSize     indexOffset{0U};
        uint32_t         vertexCount{0};
        uint32_t         indexCount{0};
        MeshIndex         meshIndex{0U};
    };

    std::unique_ptr<Buffer> mVertexBuffer;
    std::unique_ptr<Buffer> mIndexBuffer;

    std::vector<uint32_t>                                             mMeshInstances;
    std::vector<VkDrawIndexedIndirectCommand>                         mMeshInfos;
    std::vector<glm::vec4>                                            mMeshBounds;
    std::unique_ptr<Buffer>                                           mMeshBoundsBuffer;
    std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight> mMeshInfoBuffer;

    // How many new uses added/removed of a specific mesh at a index
    std::vector<utils::Pair<MeshIndex, int32_t>> mMeshUses;

    VkDeviceSize mVertexHead{0U};
    VkDeviceSize mIndexHead{0U};

    std::map<MeshHandle, Entry> mMeshes;
    uint32_t                    mMeshCount{0U};
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_CACHE_MESH_CACHE_HPP
