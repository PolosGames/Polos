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

struct shader_file
{
    std::string           custom_name;
    ShaderStage           stage;
    std::filesystem::path path;
};

struct shader_cache_create_details
{
    VkDevice                 logi_device{VK_NULL_HANDLE};
    std::vector<shader_file> shader_files;
};

class ShaderCache
{
public:
    ShaderCache();
    ~ShaderCache();

    auto Create(shader_cache_create_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto GetShaderModule(utils::string_id t_name) -> shader const*;
private:
    auto loadShaderFromFile(shader_file const& t_shader_file) -> Result<VkShaderModule>;

    VkDevice                             m_device{VK_NULL_HANDLE};
    std::vector<std::unique_ptr<shader>> m_shader_cache;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHADER_CACHE_HPP
