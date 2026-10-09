//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_SHADER_CACHE_HPP
#define POLOS_RENDERING_SHADER_CACHE_HPP

#include "polos/rendering/resources/shader.hpp"
#include "polos/utils/error_code.hpp"
#include "polos/utils/string_id.hpp"

#include <memory>
#include <vector>

namespace polos::rendering
{

struct ShaderFile
{
    ShaderStage      stage;
    utils::string_id resourceId;
};

class ShaderCache
{
public:
    struct CreationDetails
    { std::vector<ShaderFile> shaderFiles; };

    ShaderCache();
    ~ShaderCache();

    [[nodiscard]] auto Create(CreationDetails const& tDetails) -> Result<void>;
    auto               Destroy() -> void;

    auto GetShaderModule(utils::string_id tName) -> Shader const*;
private:
    std::vector<std::unique_ptr<Shader>> mShaderCache;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SHADER_CACHE_HPP
