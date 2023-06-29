#pragma once

namespace polos::resource
{
    struct image
    {
        int32        channels{};
        glm::ivec2   dimensions;
        DArray<byte> data;
    };
} // namespace polos::resource