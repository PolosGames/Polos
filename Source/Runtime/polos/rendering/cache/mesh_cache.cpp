///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/mesh_cache.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/resources/material.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/profiling.hpp"
#include "polos/utils/string_map.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <numeric>
#include <vector>

namespace polos::rendering
{

namespace
{

constexpr VkDeviceSize const kGrowthAmount{64ULL * 1024ULL * 1024ULL};

struct alignas(16) GpuMeshBounds
{
    glm::vec4 min;
    glm::vec4 max;
};

auto ComputeBounds(std::span<MeshVertex const> tVertices) -> resource::model::AABB
{
    auto const            initial = tVertices.front().position;
    resource::model::AABB bounds{.max = initial, .min = initial};
    for (auto const& vertex : tVertices.subspan(1))
    {
        bounds.max = glm::max(bounds.max, vertex.position);
        bounds.min = glm::min(bounds.min, vertex.position);
    }
    return bounds;
}

constexpr std::array<MeshVertex, 8U> kCubeVertices{
    MeshVertex{
        .position = glm::vec3(-0.5F, -0.5F, 0.5F),
    },
    MeshVertex{
        .position = glm::vec3(0.5F, -0.5F, 0.5F),
    },
    MeshVertex{
        .position = glm::vec3(0.5F, 0.5F, 0.5F),
    },
    MeshVertex{
        .position = glm::vec3(-0.5F, 0.5F, 0.5F),
    },
    MeshVertex{
        .position = glm::vec3(-0.5F, -0.5F, -0.5F),
    },
    MeshVertex{
        .position = glm::vec3(0.5F, -0.5F, -0.5F),
    },
    MeshVertex{
        .position = glm::vec3(0.5F, 0.5F, -0.5F),
    },
    MeshVertex{
        .position = glm::vec3(-0.5F, 0.5F, -0.5F),
    },
};

// clang-format off
constexpr std::array<uint16_t, 6UL * 6UL> kCubeIndices{
    4, 0, 3, 3, 7, 4,
    4, 5, 0, 0, 5, 1,
    1, 2, 0, 0, 2, 3,
    3, 2, 7, 7, 2, 6,
    6, 5, 4, 4, 7, 6,
    6, 2, 5, 5, 2, 1,
};

// clang-format on

constexpr std::size_t kIndicesPerFace = 6U;
// Each color corresponds to one row of kCubeIndices.
constexpr std::array kCubeFaceColors{
    glm::vec4(0.95F, 0.18F, 0.18F, 1.0F),
    glm::vec4(0.95F, 0.70F, 0.10F, 1.0F),
    glm::vec4(0.15F, 0.45F, 0.95F, 1.0F),
    glm::vec4(0.12F, 0.75F, 0.32F, 1.0F),
    glm::vec4(0.65F, 0.25F, 0.90F, 1.0F),
    glm::vec4(0.95F, 0.40F, 0.10F, 1.0F),
};
static_assert(kCubeFaceColors.size() * kIndicesPerFace == kCubeIndices.size());

auto CreateCubeVertices() -> std::vector<MeshVertex>
{
    std::vector<MeshVertex> vertices;
    vertices.reserve(kCubeIndices.size());
    for (std::size_t i{0}; i < kCubeIndices.size(); i += 3U)
    {
        auto const& a         = kCubeVertices[kCubeIndices[i]].position;
        auto const& b         = kCubeVertices[kCubeIndices[i + 1U]].position;
        auto const& c         = kCubeVertices[kCubeIndices[i + 2U]].position;
        auto const  normal    = glm::normalize(glm::cross(b - a, c - a));
        auto const  tangent   = normal.y != 0.0F ? glm::vec3(1.0F, 0.0F, 0.0F) : glm::vec3(normal.z, 0.0F, -normal.x);
        auto const  bitangent = glm::cross(normal, tangent);

        for (std::size_t corner{0}; corner < 3U; ++corner)
        {
            auto vertex    = kCubeVertices[kCubeIndices[i + corner]];
            vertex.normal  = normal;
            vertex.tangent = glm::vec4(tangent, 1.0F);
            vertex.texCoord =
                glm::vec2(glm::dot(vertex.position, tangent), glm::dot(vertex.position, bitangent)) + 0.5F;
            vertex.color = kCubeFaceColors[i / kIndicesPerFace];
            vertices.push_back(vertex);
        }
    }
    return vertices;
}
}// namespace

MeshCache* MeshCache::sInstance = nullptr;

auto MeshCache::Get() -> MeshCache&
{
    return *sInstance;
}

MeshCache::MeshCache()
{
    sInstance = this;
}

MeshCache::~MeshCache()
{
    sInstance = nullptr;
}

auto MeshCache::Create(CreationDetails const&) -> Result<void>
{
    mVertexBuffer = Buffer::Create(
        BufferDescription{
            .size               = static_cast<VkDeviceSize>(128U * 1024U * 1024U),// Initial 128MB
            .usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            .residence          = MemoryResidence::kDevice,
            .hostAccess         = HostAccessFlags::kNone,
            .persistentlyMapped = false,
        });

    PL_ASSERT(nullptr != mVertexBuffer, "Vertex mega buffer was not created");

    mIndexBuffer = Buffer::Create(
        BufferDescription{
            .size               = static_cast<VkDeviceSize>(64U * 1024U * 1024U),// Initial 64MB
            .usage              = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            .residence          = MemoryResidence::kDevice,
            .hostAccess         = HostAccessFlags::kNone,
            .persistentlyMapped = false,
        });

    PL_ASSERT(nullptr != mIndexBuffer, "Index mega buffer was not created");

    mMeshBoundsBuffer = Buffer::Create(
        BufferDescription{
            .size               = 4096ULL * sizeof(GpuMeshBounds),
            .usage              = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            .residence          = MemoryResidence::kDevice,
            .hostAccess         = HostAccessFlags::kSeqWrite,
            .persistentlyMapped = true,
        });

    PL_ASSERT(nullptr != mMeshBoundsBuffer, "Mesh bounds buffer was not created");

    for (size_t i{0U}; i < Settings::kMaxFramesInFlight; ++i)
    {
        mMeshInfoBuffer[i] = Buffer::Create(
            BufferDescription{
                .size               = 4096ULL * sizeof(VkDrawIndexedIndirectCommand),
                .usage              = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
                .residence          = MemoryResidence::kDevice,
                .hostAccess         = HostAccessFlags::kSeqWrite,
                .persistentlyMapped = true,
            });

        PL_ASSERT(nullptr != mMeshInfoBuffer[i], "Mesh Info buffer was not created");
    }

    {
        resource::model::MeshPrimitive cubeMesh;

        cubeMesh.name              = "mesh.DefaultCube";
        cubeMesh.material          = kDefaultMaterial;
        cubeMesh.primitiveVertices = CreateCubeVertices();
        // The generated vertices are already in triangle order.
        cubeMesh.primitiveIndices.resize(cubeMesh.primitiveVertices.size());
        std::ranges::iota(cubeMesh.primitiveIndices, std::uint32_t{0});

        std::ignore = AddMesh(cubeMesh);
    }

    {
        resource::model::MeshPrimitive quadMesh{
            .name              = "mesh.DefaultQuad",
            .primitiveVertices = {},
            .primitiveIndices  = {0U, 1U, 2U, 2U, 3U, 0U},
            .material          = {},
        };
        constexpr std::array corners{
            glm::vec2{0.0F, 0.0F},
            glm::vec2{1.0F, 0.0F},
            glm::vec2{1.0F, 1.0F},
            glm::vec2{0.0F, 1.0F},
        };
        quadMesh.primitiveVertices.reserve(corners.size());
        for (auto const& uv : corners)
        {
            quadMesh.primitiveVertices.push_back({
                .position = glm::vec3(uv - glm::vec2(0.5F), 0.0F),
                .normal   = {0.0F, 0.0F, 1.0F},
                .tangent  = {1.0F, 0.0F, 0.0F, 1.0F},
                .texCoord = uv,
            });
        }
        std::ignore = AddMesh(quadMesh);
    }

    return {};
}

auto MeshCache::Destroy() -> void
{
    mVertexBuffer.reset();
    mIndexBuffer.reset();
    mMeshBoundsBuffer.reset();
    for (auto& infoBuffer : mMeshInfoBuffer)
    {
        infoBuffer.reset();
    }
    mMeshInstances.clear();
    mMeshInfos.clear();
    mMeshUses.clear();
    mMeshes.clear();
    mVertexHead = 0U;
    mIndexHead  = 0U;
    mMeshCount  = 0U;
}

auto MeshCache::AddMesh(resource::model::MeshPrimitive& tMesh) -> MeshHandle
{
    auto meshId = utils::StrHash64Runtime(tMesh.name);
    if (mMeshes.contains(meshId))
    {
        return meshId;
    }

    auto const vertData = std::span(tMesh.primitiveVertices);
    auto const indData  = std::span(tMesh.primitiveIndices);
    PL_ASSERT(!vertData.empty() && !indData.empty(), "MeshPrimitive must contain non-empty vertices and indices");

    Entry entry;

    entry.name = meshId;

    while (mVertexHead + vertData.size_bytes() > mVertexBuffer->size)
    {
        PL_ASSERT(mVertexBuffer->Resize(mVertexBuffer->size + kGrowthAmount), "Unable to resize mesh vertices");
    }

    while (mIndexHead + indData.size_bytes() > mIndexBuffer->size)
    {
        PL_ASSERT(mIndexBuffer->Resize(mIndexBuffer->size + kGrowthAmount), "Unable to resize mesh indices");
    }

    entry.vertexOffset = mVertexHead;
    entry.indexOffset  = mIndexHead;

    PL_ASSERT(
        mVertexBuffer->Upload(std::as_bytes(vertData), entry.vertexOffset),
        "Unable to upload mesh vertices to mega buffer");
    PL_ASSERT(
        mIndexBuffer->Upload(std::as_bytes(indData), entry.indexOffset),
        "Unable to upload mesh indices to mega buffer");

    mVertexHead += vertData.size_bytes();
    mIndexHead += indData.size_bytes();

    entry.vertexCount = ToVkSize(tMesh.primitiveVertices.size());
    entry.indexCount  = ToVkSize(tMesh.primitiveIndices.size());

    mMeshInfos.push_back(
        VkDrawIndexedIndirectCommand{
            .indexCount    = ToVkSize(entry.indexCount),
            .instanceCount = 0U,
            .firstIndex    = ToVkSize(entry.indexOffset / sizeof(uint32_t)),
            .vertexOffset  = static_cast<int32_t>(entry.vertexOffset / sizeof(MeshVertex)),
            .firstInstance = 0U,
        });

    mMeshInstances.push_back(0U);

    auto const          bounds = tMesh.bounds ? *tMesh.bounds : ComputeBounds(vertData);
    GpuMeshBounds const gpuBounds{.min = glm::vec4(bounds.min, 0.0F), .max = glm::vec4(bounds.max, 0.0F)};

    entry.meshIndex = mMeshCount++;
    entry.material  = tMesh.material;

    auto const boundsOffset = entry.meshIndex * sizeof(GpuMeshBounds);
    while (boundsOffset + sizeof(GpuMeshBounds) > mMeshBoundsBuffer->size)
    {
        PL_ASSERT(mMeshBoundsBuffer->Resize(mMeshBoundsBuffer->size * 2ULL), "Unable to resize mesh bounds buffer");
    }
    mMeshBoundsBuffer->Write(&gpuBounds, sizeof(GpuMeshBounds), boundsOffset);
    mMeshes[meshId] = entry;

    return entry.name;
}

auto MeshCache::HasMesh(MeshHandle const tHandle) -> bool
{
    return mMeshes.contains(tHandle);
}

auto MeshCache::GetMeshIndexByHandle(MeshHandle const tHandle) -> MeshIndex
{
    if (!HasMesh(tHandle))
    {
        LogError("Mesh doesn't exist with name {}", utils::GetStringFromID(tHandle));
        return std::numeric_limits<uint32_t>::max();
    }

    return mMeshes[tHandle].meshIndex;
}

auto MeshCache::GetMaterialByHandle(MeshHandle tHandle) -> MaterialHandle
{
    if (!HasMesh(tHandle))
    {
        LogError("Mesh doesn't exist with name {}", utils::GetStringFromID(tHandle));
        return std::numeric_limits<uint32_t>::max();
    }

    return mMeshes[tHandle].material;
}

auto MeshCache::GetVertexBuffer() -> VkBuffer
{
    return mVertexBuffer->buffer;
}

auto MeshCache::GetIndexBuffer() -> VkBuffer
{
    return mIndexBuffer->buffer;
}

auto MeshCache::GetMeshBoundsBuffer() -> VkBuffer
{
    return mMeshBoundsBuffer->buffer;
}

auto MeshCache::GetMeshBoundsBufferSize() const -> VkDeviceSize
{
    return mMeshBoundsBuffer->size;
}

auto MeshCache::GetMeshCount() -> uint32_t
{
    return mMeshCount;
}

auto MeshCache::GetIndirectCommandBufferByFrameSlot(uint32_t const tFrameSlot) -> VkBuffer
{
    return mMeshInfoBuffer[tFrameSlot]->buffer;
}

auto MeshCache::IncreaseMeshUse(MeshHandle const tHandle) -> void
{
    if (!HasMesh(tHandle))
    {
        LogError("Mesh doesn't exist with name {}", utils::GetStringFromID(tHandle));
        return;
    }

    IncreaseMeshUseByIndex(mMeshes[tHandle].meshIndex);
}

auto MeshCache::DecreaseMeshUse(MeshHandle const tHandle) -> void
{
    if (!HasMesh(tHandle))
    {
        LogError("Mesh doesn't exist with name {}", utils::GetStringFromID(tHandle));
        return;
    }

    DecreaseMeshUseByIndex(mMeshes[tHandle].meshIndex);
}

auto MeshCache::IncreaseMeshUseByIndex(MeshIndex const tMeshIndex) -> void
{
    if (tMeshIndex >= mMeshCount)
    {
        return;
    }

    auto itr = std::ranges::find(mMeshUses, tMeshIndex, &utils::Pair<MeshIndex, int32_t>::first);
    if (itr != mMeshUses.end())
    {
        ++itr->second;
    }
    else
    {
        mMeshUses.push_back({.first = tMeshIndex, .second = 1});
    }
}

auto MeshCache::DecreaseMeshUseByIndex(MeshIndex const tMeshIndex) -> void
{
    if (tMeshIndex >= mMeshCount)
    {
        return;
    }

    auto itr = std::ranges::find(mMeshUses, tMeshIndex, &utils::Pair<MeshIndex, int32_t>::first);
    if (itr != mMeshUses.end())
    {
        --itr->second;
    }
    else
    {
        mMeshUses.push_back({.first = tMeshIndex, .second = -1});
    }
}

auto MeshCache::FlushMeshUses(uint32_t const tFrameSlot) -> void
{
    POLOS_PROFILE_ZONE("Update indirect draw commands");
    if (!mMeshUses.empty())
    {
        for (auto const& meshUse : mMeshUses)
        {
            int32_t const totalUse = std::max(static_cast<int32_t>(mMeshInstances[meshUse.first]) + meshUse.second, 0);
            mMeshInstances[meshUse.first] = static_cast<uint32_t>(totalUse);
        }

        uint32_t runningOffset{0U};
        for (size_t i{0U}; i < mMeshInfos.size(); ++i)
        {
            mMeshInfos[i].instanceCount = 0U;
            mMeshInfos[i].firstInstance = runningOffset;
            runningOffset += mMeshInstances[i];
        }

        mMeshUses.clear();
    }

    if (mMeshCount > 0U)
    {
        auto const requiredBytes = mMeshCount * sizeof(VkDrawIndexedIndirectCommand);
        PL_ASSERT(requiredBytes <= mMeshInfoBuffer[tFrameSlot]->size, "Exceeded indirect command buffer capacity");
        mMeshInfoBuffer[tFrameSlot]->Write(mMeshInfos.data(), requiredBytes);
    }
}


}// namespace polos::rendering
