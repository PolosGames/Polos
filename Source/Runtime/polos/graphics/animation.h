#pragma once

#include "polos/graphics/texture.h"

namespace polos
{
struct base_animation
{
    int64                   id;
    std::vector<TextureRef> frames;
    bool                    loop{true};
};

template<std::size_t Size> struct animation : public base_animation
{
    animation()
    {
        frames.reserve(Size);
    }
};

}// namespace polos
