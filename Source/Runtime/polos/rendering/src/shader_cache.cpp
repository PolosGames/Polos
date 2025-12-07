//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/shader_cache.hpp"

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"

#include <array>

namespace polos::rendering
{

ShaderCache::ShaderCache()  = default;
ShaderCache::~ShaderCache() = default;

auto ShaderCache::Create(shader_cache_create_details const& t_details) -> Result<void>
{
    m_device = t_details.logi_device;

    for (auto const& shader_file : t_details.shader_files)
    {
        auto shader_result = loadShaderFromFile(shader_file.first, shader_file.second);
        if (!shader_result.has_value())
        {
            return ErrorType{shader_result.error()};
        }

        m_shader_cache.push_back(std::move(*shader_result));
    }

    return {};
}

auto ShaderCache::Destroy() -> Result<void>
{
    for (auto const& shader : m_shader_cache) { vkDestroyShaderModule(m_device, shader.module, nullptr); }
    return {};
}

auto ShaderCache::GetShaderModule(std::string const& t_name) -> shader const&
{
    auto const itr = std::ranges::find_if(m_shader_cache, [&t_name](shader const& t_shader) {
        return t_shader.name == t_name;
    });
    assert(itr != m_shader_cache.end() && "Shader module not loaded to engine!");

    return *itr;
}

auto ShaderCache::loadShaderFromFile(std::string_view const t_shader_custom_name, std::filesystem::path const& t_path)
    -> Result<shader>
{
    auto shader_code = fs::ReadFile(t_path);
    if (!shader_code.has_value())
    {
        return ErrorType{RenderingErrc::kFailedCreateShaderModule};
    }

    if (shader_code->data.size() % 4 != 0)// ensure we can convert to std::uint32_t
    {
        LogError("The SPIR-V code that has been read cannot be used for shader creation!");
        return ErrorType{RenderingErrc::kFailedCreateShaderModule};
    }

    union byte_to_uint32
    {
        std::array<std::byte, sizeof(std::uint32_t)> array;
        std::uint32_t                                opcode{0U};
    } converter;

    std::vector<std::uint32_t> code(shader_code->data.size() / 4);
    for (std::size_t i{0U}; i < code.size(); ++i)
    {
        std::size_t const current_uint = i * 4;
        converter.array[0]             = shader_code->data[current_uint + 0];
        converter.array[1]             = shader_code->data[current_uint + 1];
        converter.array[2]             = shader_code->data[current_uint + 2];
        converter.array[3]             = shader_code->data[current_uint + 3];

        code[i] = converter.opcode;
    }

    VkShaderModuleCreateInfo const create_info{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .codeSize = shader_code->data.size(),
        .pCode    = code.data(),
    };

    VkShaderModule shader_module{VK_NULL_HANDLE};

    if (VkResult const result = vkCreateShaderModule(m_device, &create_info, nullptr, &shader_module);
        result != VK_SUCCESS)
    {
        return ErrorType{RenderingErrc::kFailedCreateShaderModule};
    }

    return shader{
        .name   = std::string{t_shader_custom_name},
        .module = shader_module,
    };
}

}// namespace polos::rendering