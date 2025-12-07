//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHADER_CACHE_HPP
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHADER_CACHE_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/shader.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace polos::rendering
{

struct shader_cache_create_details
{
    VkDevice                                                   logi_device{VK_NULL_HANDLE};
    std::vector<std::pair<std::string, std::filesystem::path>> shader_files;
};

class ShaderCache
{
public:
    ShaderCache();
    ~ShaderCache();

    auto Create(shader_cache_create_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto GetShaderModule(std::string const& t_name) -> shader const&;
private:
    auto loadShaderFromFile(std::string_view const t_shader_custom_name, std::filesystem::path const& t_path)
        -> Result<shader>;

    VkDevice            m_device{VK_NULL_HANDLE};
    std::vector<shader> m_shader_cache;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SHADER_CACHE_HPP
