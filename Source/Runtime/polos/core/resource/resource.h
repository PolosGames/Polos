#pragma once

#include "polos/core/resource/types/image.h"
#include "polos/core/resource/types/model.h"

#include "polos/core/resource/importers/image_importer_stb.h"
#include "polos/core/resource/importers/gltf_importer.h"

#include "polos/core/resource/import_options.h"
#include "polos/core/resource/options/image_import_options.h"
#include "polos/core/resource/options/model_import_options.h"

namespace polos::resource
{
    template<typename T>
    auto ImportResource(std::string const& p_Path, import_options<T> p_Options) -> ResourceHandle;

    template<typename T>
    auto GetResource(std::string const& p_Name) -> ResourceHandle;

    template<typename T>
    auto GetRawResource(std::string const& p_Name) -> T*;

    template<typename T>
    auto DestroyResource(std::string const& p_Name) -> void;
} // namespace polos::resource

#include "resource.inl"
