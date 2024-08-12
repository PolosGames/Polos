#pragma once

#include "polos/core/resource/resource_loader.h"
#include "polos/core/resource/types/model.h"
#include "polos/core/resource/options/model_import_options.h"

namespace Assimp
{
    class Importer;
    class BaseImporter;
    class IOSystem;
};

namespace polos::resource
{
    class GltfImporter : public ResourceLoader<model>
    {
        using Base = ResourceLoader<model>;
    public:
        GltfImporter();
    private:
        auto Import(std::string p_Path, import_options<model> p_Options, model* p_Ptr) -> model*;
        auto CanImport(std::string p_Path) -> bool;

        UniquePtr<Assimp::Importer> m_Importer;
        Assimp::IOSystem*           m_IOHandle;
    };
} // namespace polos::resource
