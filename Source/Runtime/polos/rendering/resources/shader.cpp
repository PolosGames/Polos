///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/resources/shader.hpp"

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"

#include <cstring>
#include <vector>

namespace polos::rendering
{

auto Shader::Create(std::span<std::byte const> tData, ShaderStage tStage) -> std::shared_ptr<Shader>
{
    if (tData.empty() || tData.size() % sizeof(std::uint32_t) != 0U)
    {
        LogError("Invalid SPIR-V size: {} bytes.", tData.size());
        return nullptr;
    }

    // Vulkan expects word-aligned SPIR-V. The byte span may be unaligned.
    std::vector<std::uint32_t> code(tData.size() / sizeof(std::uint32_t));
    std::memcpy(code.data(), tData.data(), tData.size());
    VkShaderModuleCreateInfo const createInfo{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .codeSize = tData.size(),
        .pCode    = code.data(),
    };

    VkDevice const device = VulkanDevice::Get().logiDevice;
    auto shader = std::shared_ptr<Shader>{new Shader{.stage = tStage}, [device](Shader* tShader) {
                                              vkDestroyShaderModule(device, tShader->module, nullptr);
                                              delete tShader;
                                          }};

    VkResult const result = vkCreateShaderModule(device, &createInfo, nullptr, &shader->module);
    if (result != VK_SUCCESS)
    {
        shader->module = VK_NULL_HANDLE;
        LogError("Failed to create shader module: VkResult {}.", static_cast<int>(result));
        return nullptr;
    }
    return shader;
}

}// namespace polos::rendering
