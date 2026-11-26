///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_CACHE_MODEL_CACHE_HPP
#define POLOS_RENDERING_CACHE_MODEL_CACHE_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/resources/model.hpp"

#include <unordered_map>

namespace polos::rendering
{

class POLOS_API ModelCache
{
public:
    [[nodiscard]] static auto Get() -> ModelCache&;

    struct CreationDetails
    {
    };

    ModelCache();
    ~ModelCache();

    ModelCache(ModelCache const&)                    = delete;
    auto operator=(ModelCache const&) -> ModelCache& = delete;

    auto Create(CreationDetails const&) -> Result<void>;
    auto Destroy() -> void;

    auto               AddModel(ResourceId tId, Model tModel) -> void;
    [[nodiscard]] auto GetModel(ResourceId tId) const -> Model const*;
private:
    static ModelCache* sInstance;

    std::unordered_map<ResourceId, Model> mModels;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_CACHE_MODEL_CACHE_HPP
