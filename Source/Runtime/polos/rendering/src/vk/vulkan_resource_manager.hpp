///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_VULKAN_RESOURCE_MANAGER_HPP
#define POLOS_RENDERING_SRC_VK_VULKAN_RESOURCE_MANAGER_HPP

#include "polos/communication/error_code.hpp"
#include "resources/allocated_buffer.hpp"
#include "resources/allocated_image.hpp"
#include "resources/buffer_description.hpp"
#include "resources/image_description.hpp"

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace polos::rendering
{

class VulkanContext;
class VulkanDevice;
class VulkanSwapchain;

struct alignas(64) ResourceManagerCreateDetails// NOLINT
{
    VkDevice         device;
    VmaAllocator     allocator;
    VulkanSwapchain* swapchain;
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

    static auto Instance() -> VulkanResourceManager*;

    auto Create(ResourceManagerCreateDetails const& tDetails) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto CreateBuffer(BufferDescription const& tDesc) -> Result<std::int32_t>;
    auto DestroyBuffer(std::int32_t tResourceId) -> void;

    auto CreateImage(ImageDescription const& tDesc) -> Result<std::int32_t>;
    auto DestroyImage(std::int32_t tResourceId) -> void;

    auto LoadImageResourceToVkImage(char const*          tResourcePath,
                                    VkFormat             tFormat,
                                    VkImageUsageFlags    tUsage,
                                    VkImageLayout        tFinalLayout,
                                    VkPipelineStageFlags tFinalPipelineStage) -> Result<std::int32_t>;

    auto GetBuffer(std::int32_t tResourceId) -> VkBuffer;
    auto GetImage(std::int32_t tResourceId) -> VkImage;
    auto GetAllocatedBuffer(std::int32_t tResourceId) -> AllocatedBuffer*;
    auto GetAllocatedImage(std::int32_t tResourceId) -> AllocatedImage*;
private:
    static std::int32_t           sResourceId;
    static VulkanResourceManager* sInstance;

    VkDevice         mDevice{VK_NULL_HANDLE};
    VmaAllocator     mAllocator{VK_NULL_HANDLE};
    VulkanSwapchain* mSwapchain{nullptr};

    std::vector<std::unique_ptr<AllocatedImage>>  mImages;
    std::vector<std::unique_ptr<AllocatedBuffer>> mBuffers;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_VK_VULKAN_RESOURCE_MANAGER_HPP
