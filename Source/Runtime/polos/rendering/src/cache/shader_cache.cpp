//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "cache/shader_cache.hpp"

#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"

#include <array>
#include <memory>

namespace polos::rendering
{

ShaderCache::ShaderCache()  = default;
ShaderCache::~ShaderCache() = default;

auto ShaderCache::Create(ShaderCacheCreateDetails const& tDetails) -> Result<void>
{
    mDevice = tDetails.logiDevice;

    for (auto const& ShaderFile : tDetails.shaderFiles)
    {
        auto shader_result = loadShaderFromFile(ShaderFile);
        if (!shader_result.has_value())
        {
            return ErrorType{shader_result.error()};
        }

        VkShaderModule shader_module = shader_result.value();

        // NOLINTNEXTLINE
        mShaderCache.emplace_back(new Shader{
            .name   = utils::StrHash64(ShaderFile.customName),
            .stage  = ShaderFile.stage,
            .module = shader_module,
        });
    }

    return {};
}

auto ShaderCache::Destroy() -> Result<void>
{
    for (auto const& Shader : mShaderCache) { vkDestroyShaderModule(mDevice, Shader->module, nullptr); }
    return {};
}

auto ShaderCache::GetShaderModule(utils::string_id const tName) -> Shader const*
{
    auto const itr = std::ranges::find_if(mShaderCache, [&tName](std::unique_ptr<Shader> const& tShader) {
        return tShader->name == tName;
    });
    if (itr == mShaderCache.end())
    {
        LogError("Shader module not loaded to engine!");
        return nullptr;
    }

    return itr->get();
}

auto ShaderCache::loadShaderFromFile(ShaderFile const& tShaderFile) -> Result<VkShaderModule>
{
    auto shader_code = fs::ReadFile(tShaderFile.path);
    if (!shader_code.has_value())
    {
        return ErrorType{RenderingErrc::kFailedCreateShaderModule};
    }

    if (shader_code->data.size() % 4 != 0)// ensure we can convert to std::uint32_t
    {
        LogError("The SPIR-V code that has been read cannot be used for Shader creation!");
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

    if (VkResult const result = vkCreateShaderModule(mDevice, &create_info, nullptr, &shader_module);
        result != VK_SUCCESS)
    {
        return ErrorType{RenderingErrc::kFailedCreateShaderModule};
    }

    return shader_module;
}

}// namespace polos::rendering