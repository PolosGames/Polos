#pragma once

#include "polos/core/resource/resource_loader.h"
#include "polos/core/resource/types/image.h"
#include "polos/core/resource/options/image_import_options.h"

namespace polos::resource
{
    class ImageImporterStb : public ResourceLoader<image>
    {
    public:
        ImageImporterStb();
    private:
        auto Import(std::string p_Path, import_options<image> p_Options, image* p_Ptr) -> image*;
        auto CanImport(std::string p_Path) -> bool;
    };
} // namespace polos::resource