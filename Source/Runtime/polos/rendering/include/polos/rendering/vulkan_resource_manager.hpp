//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/shader.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace polos::rendering
{

class VulkanContext;
class VulkanDevice;
class VulkanSwapchain;
struct texture_2d;

struct resource_manager_create_details
{
    // Custom name, Path
    VkDevice         device;
    VulkanSwapchain* swapchain;

    std::vector<std::pair<std::string, std::filesystem::path>> shader_files;
};

class VulkanResourceManager
{
public:
    VulkanResourceManager();
    ~VulkanResourceManager();

    VulkanResourceManager(VulkanResourceManager const&)            = delete;
    VulkanResourceManager(VulkanResourceManager&&)                 = delete;
    VulkanResourceManager& operator=(VulkanResourceManager const&) = delete;
    VulkanResourceManager& operator=(VulkanResourceManager&&)      = delete;

    static auto Instance() -> VulkanResourceManager&;

    auto Create(resource_manager_create_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto GetShaderModule(std::string const& t_name) -> VkShaderModule;
    auto GetTexture(std::string const& t_name) -> VkShaderModule;
private:
    auto loadShaderFromFile(std::string_view const t_shader_custom_name, std::filesystem::path const& t_path)
        -> Result<std::pair<std::string, shader>>;
    auto onFramebufferResize() -> void;

    friend class RenderContext;
    static VulkanResourceManager* s_instance;

    VkDevice         m_device;
    VulkanSwapchain* m_swapchain;

    std::unordered_map<std::string, shader>  m_shader_cache;
    std::vector<std::shared_ptr<texture_2d>> m_textures;
};

using VRM = VulkanResourceManager;

VulkanResourceManager& GetVRM();

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_VULKAN_RESOURCE_MANAGER_HPP_
