//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_resource_manager.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"

namespace polos::rendering
{

VulkanResourceManager* VulkanResourceManager::s_instance = nullptr;

VulkanResourceManager::VulkanResourceManager()  = default;
VulkanResourceManager::~VulkanResourceManager() = default;

auto VulkanResourceManager::Instance() -> VulkanResourceManager&
{
    return *s_instance;
}

auto VulkanResourceManager::Create(resource_manager_create_details const& t_details) -> Result<void>
{
    for (auto const& shader_file : t_details.shader_files)
    {
        auto shader_result = load_shader_from_file(shader_file.first, shader_file.second);
        if (!shader_result.has_value())
        {
            return ErrorType{shader_result.error()};
        }

        m_shader_cache.try_emplace(shader_result->first, shader_result->second.module);
    }
    return {};
}

auto VulkanResourceManager::Destroy() -> Result<void>
{
    return {};
}

auto VulkanResourceManager::GetShaderModule(std::string const& t_name) -> VkShaderModule
{
    auto it = m_shader_cache.find(t_name);
    if (it == m_shader_cache.end())
    {
        LogWarn("Requested shader not found in cache");
        return VK_NULL_HANDLE;
    }

    return it->second.module;
}

auto VulkanResourceManager::load_shader_from_file(
    std::string_view const       t_shader_custom_name,
    std::filesystem::path const& t_path) -> Result<std::pair<std::string, shader>>
{
    auto shader_code = fs::ReadFile(t_path);
    if (!shader_code.has_value())
    {
        return ErrorType{RenderingErrc::kShaderModuleNotCreated};
    }

    VkShaderModuleCreateInfo create_info{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .codeSize = shader_code->data.size(),
        .pCode    = reinterpret_cast<std::uint32_t const*>(shader_code->data.data()),
    };

    VkShaderModule shader_module{VK_NULL_HANDLE};

    VkResult result = vkCreateShaderModule(m_device, &create_info, nullptr, &shader_module);
    if (result != VK_SUCCESS)
    {
        return ErrorType{RenderingErrc::kShaderModuleNotCreated};
    }

    return std::pair<std::string, shader>{
        t_shader_custom_name.data(),
        shader{
            .module = shader_module,
        }};
}

VulkanResourceManager& GetVRM()
{
    return VRM::Instance();
}

}// namespace polos::rendering
