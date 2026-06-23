//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHADER_CACHE_HPP
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHADER_CACHE_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/shader.hpp"
#include "polos/utils/string_id.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace polos::rendering
{

struct ShaderFile
{
    std::string           custom_name;
    ShaderStage           stage;
    std::filesystem::path path;
};

struct ShaderCacheCreateDetails
{
    VkDevice                 logi_device{VK_NULL_HANDLE};
    std::vector<ShaderFile> shader_files;
};

class ShaderCache
{
public:
    ShaderCache();
    ~ShaderCache();

    auto Create(ShaderCacheCreateDetails const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto GetShaderModule(utils::string_id t_name) -> Shader const*;
private:
    auto loadShaderFromFile(ShaderFile const& t_shader_file) -> Result<VkShaderModule>;

    VkDevice                             m_device{VK_NULL_HANDLE};
    std::vector<std::unique_ptr<Shader>> m_shader_cache;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHADER_CACHE_HPP
