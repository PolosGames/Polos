//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_SRC_CACHE_SHADER_CACHE_HPP
#define POLOS_RENDERING_SRC_CACHE_SHADER_CACHE_HPP

#include "polos/communication/error_code.hpp"
#include "polos/utils/string_id.hpp"
#include "resources/shader.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace polos::rendering
{

struct ShaderFile
{
    std::string           customName;
    ShaderStage           stage;
    std::filesystem::path path;
};

struct ShaderCacheCreateDetails
{
    VkDevice                logiDevice{VK_NULL_HANDLE};
    std::vector<ShaderFile> shaderFiles;
};

class ShaderCache
{
public:
    ShaderCache();
    ~ShaderCache();

    auto Create(ShaderCacheCreateDetails const& tDetails) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto GetShaderModule(utils::string_id tName) -> Shader const*;
private:
    auto loadShaderFromFile(ShaderFile const& tShaderFile) -> Result<VkShaderModule>;

    VkDevice                             mDevice{VK_NULL_HANDLE};
    std::vector<std::unique_ptr<Shader>> mShaderCache;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_CACHE_SHADER_CACHE_HPP
