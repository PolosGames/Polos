#pragma once

#include "polos/core/resource/resource_loader.h"
#include "polos/core/resource/types/image.h"

namespace polos::resource
{
    class ImageLoaderStb : public ResourceLoader<image>
    {
    public:
        ImageLoaderStb();
    private:
        auto Load(std::string p_Path, image* p_Ptr) -> image*;
        auto CanLoad(std::string p_Path) -> bool;
    };
} // namespace polos::resource