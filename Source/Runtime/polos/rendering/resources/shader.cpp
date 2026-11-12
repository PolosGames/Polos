///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/resources/shader.hpp"

#include "polos/filesystem/file.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/vk/vulkan_device.hpp"
#include "polos/resource/resource_manager.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"

#include <cstring>
#include <vector>

namespace polos::rendering
{

auto Shader::Create(utils::string_id tResourceId, ShaderStage tStage) -> std::shared_ptr<Shader>
{
    auto file = resource::ResourceManager::Get().GetFile(tResourceId);
    if (!file)
    {
        LogError("Could not get preloaded shader resource '{}'.", tResourceId);
        return nullptr;
    }

    if (file->data.empty() || file->data.size() % sizeof(std::uint32_t) != 0)
    {
        LogError("Invalid SPIR-V size for shader resource '{}'.", tResourceId);
        return nullptr;
    }

    std::vector<std::uint32_t> code(file->data.size() / sizeof(std::uint32_t));
    std::memcpy(code.data(), file->data.data(), file->data.size());
    VkShaderModuleCreateInfo const createInfo{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0U,
        .codeSize = file->data.size(),
        .pCode    = code.data(),
    };

    VkDevice const device = VulkanDevice::Get().logiDevice;
    auto shader = std::shared_ptr<Shader>{new Shader{.name = tResourceId, .stage = tStage}, [device](Shader* tShader) {
                                              vkDestroyShaderModule(device, tShader->module, nullptr);
                                              delete tShader;
                                          }};

    VkShaderModule module{VK_NULL_HANDLE};
    PL_ASSERT(
        VK_SUCCESS == vkCreateShaderModule(device, &createInfo, nullptr, &module),
        "Failed to create shader {}",
        utils::GetStringFromID(tResourceId));

    shader->module = module;
    return shader;
}

}// namespace polos::rendering
