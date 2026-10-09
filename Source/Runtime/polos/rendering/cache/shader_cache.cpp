//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/cache/shader_cache.hpp"

#include "polos/filesystem/file.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/resource/resource_manager.hpp"

#include <array>
#include <memory>

namespace polos::rendering
{

namespace
{
auto LoadShaderFromFile(ShaderFile const& tShaderFile) -> VkShaderModule
{
    auto shader_code = resource::ResourceManager::Instance().GetFile(tShaderFile.resourceId);
    if (!shader_code)
    {
        LogError("Could not get preloaded shader resource '{}'.", tShaderFile.resourceId);
        return VK_NULL_HANDLE;
    }

    if (shader_code->data.size() % 4 != 0)// ensure we can convert to std::uint32_t
    {
        LogError("The SPIR-V code that has been read cannot be used for Shader creation!");
        return VK_NULL_HANDLE;
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

    if (VkResult const result =
            vkCreateShaderModule(RenderContext::GetVulkanDevice().logiDevice, &create_info, nullptr, &shader_module);
        result != VK_SUCCESS)
    {
        LogError("Failed to create shader module for '{}'.", tShaderFile.resourceId);
        return VK_NULL_HANDLE;
    }

    return shader_module;
}
}// namespace

ShaderCache::ShaderCache()  = default;
ShaderCache::~ShaderCache() = default;

auto ShaderCache::Create(CreationDetails const& tDetails) -> Result<void>
{
    for (auto const& ShaderFile : tDetails.shaderFiles)
    {
        VkShaderModule shader_module = LoadShaderFromFile(ShaderFile);
        if (VK_NULL_HANDLE == shader_module)
        {
            Destroy();
            return ErrorType{RenderingErrc::kFailedCreateShaderModule};
        }

        // NOLINTNEXTLINE
        mShaderCache.emplace_back(new Shader{
            .name   = ShaderFile.resourceId,
            .stage  = ShaderFile.stage,
            .module = shader_module,
        });
    }

    return {};
}

auto ShaderCache::Destroy() -> void
{
    for (auto const& Shader : mShaderCache)
    {
        vkDestroyShaderModule(RenderContext::GetVulkanDevice().logiDevice, Shader->module, nullptr);
    }
    mShaderCache.clear();
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

}// namespace polos::rendering
