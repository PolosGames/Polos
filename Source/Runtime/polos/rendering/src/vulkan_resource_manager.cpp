//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/vulkan_resource_manager.hpp"

#include "polos/communication/error_code.hpp"
#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_error_domain.hpp"
#include "polos/rendering/texture_2d.hpp"
#include "polos/rendering/vulkan_swapchain.hpp"

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
    m_device    = t_details.device;
    m_swapchain = t_details.swapchain;

    for (auto const& shader_file : t_details.shader_files)
    {
        auto shader_result = load_shader_from_file(shader_file.first, shader_file.second);
        if (!shader_result.has_value())
        {
            return ErrorType{shader_result.error()};
        }

        m_shader_cache.try_emplace(shader_result->first, shader_result->second.module);
    }

    VkFormat const   sc_img_fmt = m_swapchain->GetSurfaceFormat().format;
    VkExtent2D const sc_img_ext = m_swapchain->GetExtent();

    // Create swapchain images as textures if they have been created.
    // always make sure they are the first n images.
    for (std::uint32_t i{0U}; i < m_swapchain->GetImageCount(); ++i)
    {
        m_textures.push_back(
            std::make_shared<texture_2d>(
                m_swapchain->GetImage(i),
                m_swapchain->GetImageView(i),
                sc_img_fmt,
                sc_img_ext,
                VK_SAMPLE_COUNT_1_BIT));
    }

    return {};
}

auto VulkanResourceManager::Destroy() -> Result<void>
{
    for (auto const& [name, shader] : m_shader_cache) { vkDestroyShaderModule(m_device, shader.module, nullptr); }
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

    if (shader_code->data.size() % 4 != 0)// ensure we can convert to std::uint32_t
    {
        LogError("The SPIR-V code that has been read cannot be used for shader creation!");
        return ErrorType{RenderingErrc::kShaderModuleNotCreated};
    }

    union byte_to_uint32
    {
        std::byte     array[4];
        std::uint32_t opcode;
    } converter;

    std::vector<std::uint32_t> code(shader_code->data.size() / 4);
    for (std::size_t i{0U}; i < code.size(); ++i)
    {
        std::size_t current_uint = i * 4;
        converter.array[0]       = shader_code->data[current_uint + 0];
        converter.array[1]       = shader_code->data[current_uint + 1];
        converter.array[2]       = shader_code->data[current_uint + 2];
        converter.array[3]       = shader_code->data[current_uint + 3];

        code[i] = converter.opcode;
    }

    VkShaderModuleCreateInfo create_info{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .codeSize = shader_code->data.size(),
        .pCode    = code.data(),
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
