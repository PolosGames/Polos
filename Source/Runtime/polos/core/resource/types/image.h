#pragma once

namespace polos::resource
{
    struct alignas(64) image
    {
        int32        channels{};
        glm::ivec2   dimensions;
        DArray<byte> data;
    };
} // namespace polos::resource