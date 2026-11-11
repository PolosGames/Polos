///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/cache/mesh_cache.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk/vulkan_utils.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <numeric>
#include <vector>

namespace polos::rendering
{

namespace
{

constexpr VkDeviceSize const kGrowthAmount{64ULL * 1024ULL * 1024ULL};

constexpr std::array<PrimitiveVertex, 8U> kCubeVertices{
    PrimitiveVertex{
        .position = glm::vec3(-0.5F, -0.5F, 0.5F),
    },
    PrimitiveVertex{
        .position = glm::vec3(0.5F, -0.5F, 0.5F),
    },
    PrimitiveVertex{
        .position = glm::vec3(0.5F, 0.5F, 0.5F),
    },
    PrimitiveVertex{
        .position = glm::vec3(-0.5F, 0.5F, 0.5F),
    },
    PrimitiveVertex{
        .position = glm::vec3(-0.5F, -0.5F, -0.5F),
    },
    PrimitiveVertex{
        .position = glm::vec3(0.5F, -0.5F, -0.5F),
    },
    PrimitiveVertex{
        .position = glm::vec3(0.5F, 0.5F, -0.5F),
    },
    PrimitiveVertex{
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

auto CreateCubeVertices() -> std::vector<PrimitiveVertex>
{
    std::vector<PrimitiveVertex> vertices;
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
        resource::model::MeshPart cubeMesh;

        cubeMesh.name              = kPolosDefaultCube;
        cubeMesh.material          = resource::model::Material{};
        cubeMesh.primitiveVertices = CreateCubeVertices();
        // The generated vertices are already in triangle order.
        cubeMesh.primitiveIndices.resize(cubeMesh.primitiveVertices.size());
        std::ranges::iota(cubeMesh.primitiveIndices, std::uint32_t{0});

        std::ignore = AddMesh(cubeMesh);
    }

    return {};
}

auto MeshCache::Destroy() -> void
{
    mVertexBuffer.reset();
    mIndexBuffer.reset();
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

auto MeshCache::AddMesh(resource::model::MeshPart& tMesh) -> MeshHandle
{
    if (sInstance->mMeshes.contains(tMesh.name))
    {
        return tMesh.name;
    }

    auto const vertData = std::span(tMesh.primitiveVertices);
    auto const indData  = std::span(tMesh.primitiveIndices);
    PL_ASSERT(!vertData.empty() && !indData.empty(), "MeshPart must contain non-empty vertices and indices");

    Entry entry;

    entry.name = tMesh.name;

    while (sInstance->mVertexHead + vertData.size_bytes() > sInstance->mVertexBuffer->size)
    {
        PL_ASSERT(
            sInstance->mVertexBuffer->Resize(sInstance->mVertexBuffer->size + kGrowthAmount),
            "Unable to resize mesh vertices");
    }

    while (sInstance->mIndexHead + indData.size_bytes() > sInstance->mIndexBuffer->size)
    {
        PL_ASSERT(
            sInstance->mIndexBuffer->Resize(sInstance->mIndexBuffer->size + kGrowthAmount),
            "Unable to resize mesh indices");
    }

    entry.vertexOffset = sInstance->mVertexHead;
    entry.indexOffset  = sInstance->mIndexHead;

    PL_ASSERT(
        sInstance->mVertexBuffer->Upload(std::as_bytes(vertData), entry.vertexOffset),
        "Unable to upload mesh vertices to mega buffer");
    PL_ASSERT(
        sInstance->mIndexBuffer->Upload(std::as_bytes(indData), entry.indexOffset),
        "Unable to upload mesh indices to mega buffer");

    sInstance->mVertexHead += vertData.size_bytes();
    sInstance->mIndexHead += indData.size_bytes();

    entry.vertexCount = ToVkSize(tMesh.primitiveVertices.size());
    entry.indexCount  = ToVkSize(tMesh.primitiveIndices.size());

    sInstance->mMeshInfos.push_back(
        VkDrawIndexedIndirectCommand{
            .indexCount    = ToVkSize(entry.indexCount),
            .instanceCount = 0U,
            .firstIndex    = ToVkSize(entry.indexOffset / sizeof(uint32_t)),
            .vertexOffset  = static_cast<int32_t>(entry.vertexOffset / sizeof(PrimitiveVertex)),
            .firstInstance = 0U,
        });

    sInstance->mMeshInstances.push_back(0U);

    entry.meshIndex = sInstance->mMeshCount++;

    sInstance->mMeshes[tMesh.name] = entry;

    return entry.name;
}

auto MeshCache::HasMesh(MeshHandle const tHandle) -> bool
{
    return sInstance->mMeshes.contains(tHandle);
}

auto MeshCache::GetMeshIndexByHandle(MeshHandle const tHandle) -> uint32_t
{
    if (!HasMesh(tHandle))
    {
        LogError("Mesh doesn't exist with name {}", utils::GetStringFromID(tHandle));
        return std::numeric_limits<uint32_t>::max();
    }

    return sInstance->mMeshes[tHandle].meshIndex;
}

auto MeshCache::GetVertexBuffer() -> VkBuffer
{
    return sInstance->mVertexBuffer->buffer;
}

auto MeshCache::GetIndexBuffer() -> VkBuffer
{
    return sInstance->mIndexBuffer->buffer;
}

auto MeshCache::GetMeshCount() -> uint32_t
{
    return sInstance->mMeshCount;
}

auto MeshCache::GetIndirectCommandBufferByFrameSlot(uint32_t const tFrameSlot) -> VkBuffer
{
    return sInstance->mMeshInfoBuffer[tFrameSlot]->buffer;
}

auto MeshCache::IncreaseMeshUse(MeshHandle const tHandle) -> void
{
    if (!HasMesh(tHandle))
    {
        LogError("Mesh doesn't exist with name {}", utils::GetStringFromID(tHandle));
        return;
    }

    IncreaseMeshUseByIndex(sInstance->mMeshes[tHandle].meshIndex);
}

auto MeshCache::DecreaseMeshUse(MeshHandle const tHandle) -> void
{
    if (!HasMesh(tHandle))
    {
        LogError("Mesh doesn't exist with name {}", utils::GetStringFromID(tHandle));
        return;
    }

    DecreaseMeshUseByIndex(sInstance->mMeshes[tHandle].meshIndex);
}

auto MeshCache::IncreaseMeshUseByIndex(uint32_t const tMeshIndex) -> void
{
    if (tMeshIndex >= sInstance->mMeshCount)
    {
        return;
    }

    auto itr = std::ranges::find(sInstance->mMeshUses, tMeshIndex, &utils::Pair<uint32_t, int32_t>::first);
    if (itr != sInstance->mMeshUses.end())
    {
        ++itr->second;
    }
    else
    {
        sInstance->mMeshUses.push_back({.first = tMeshIndex, .second = 1});
    }
}

auto MeshCache::DecreaseMeshUseByIndex(uint32_t const tMeshIndex) -> void
{
    if (tMeshIndex >= sInstance->mMeshCount)
    {
        return;
    }

    auto itr = std::ranges::find(sInstance->mMeshUses, tMeshIndex, &utils::Pair<uint32_t, int32_t>::first);
    if (itr != sInstance->mMeshUses.end())
    {
        --itr->second;
    }
    else
    {
        sInstance->mMeshUses.push_back({.first = tMeshIndex, .second = -1});
    }
}

auto MeshCache::FlushMeshUses(uint32_t const tFrameSlot) -> void
{
    if (!sInstance->mMeshUses.empty())
    {
        for (auto const& meshUse : sInstance->mMeshUses)
        {
            int32_t const totalUse =
                std::max(static_cast<int32_t>(sInstance->mMeshInstances[meshUse.first]) + meshUse.second, 0);
            sInstance->mMeshInstances[meshUse.first] = static_cast<uint32_t>(totalUse);
        }

        uint32_t runningOffset{0U};
        for (size_t i{0U}; i < sInstance->mMeshInfos.size(); ++i)
        {
            sInstance->mMeshInfos[i].instanceCount = 0U;
            sInstance->mMeshInfos[i].firstInstance = runningOffset;
            runningOffset += sInstance->mMeshInstances[i];
        }

        sInstance->mMeshUses.clear();
    }

    if (sInstance->mMeshCount > 0U)
    {
        auto const requiredBytes = sInstance->mMeshCount * sizeof(VkDrawIndexedIndirectCommand);
        PL_ASSERT(
            requiredBytes <= sInstance->mMeshInfoBuffer[tFrameSlot]->size,
            "Exceeded indirect command buffer capacity");
        sInstance->mMeshInfoBuffer[tFrameSlot]->Write(sInstance->mMeshInfos.data(), requiredBytes);
    }
}


}// namespace polos::rendering
