///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/ssbo/quad_instance.hpp"

#include "polos/rendering/vk/settings.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{

auto CreateQuadInstanceStorageBuffer() -> std::unique_ptr<Buffer>
{
    auto buffer = Buffer::Create(
        BufferDescription{
            .size       = sizeof(QuadInstance) * Settings::kMaxQuads,
            .usage      = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            .residence  = MemoryResidence::kHost,
            .hostAccess = HostAccessFlags::kSeqWrite,
        });

    PL_ASSERT(nullptr != buffer, "Could not create instance buffers");

    return buffer;
}

}// namespace polos::rendering
