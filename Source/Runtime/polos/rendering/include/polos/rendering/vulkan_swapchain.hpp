//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_VULKAN_SWAPCHAIN_HPP_
#define POLOS_RENDERING_INCLUDE_VULKAN_SWAPCHAIN_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/rendering/common.hpp"
#include "polos/rendering/module_macros.hpp"

#include <vector>

namespace polos::rendering
{

class VulkanDevice;

struct swapchain_create_details
{
    VulkanDevice const* device;
    VkPhysicalDevice    phys_device{VK_NULL_HANDLE};
    VkSurfaceKHR        surface{VK_NULL_HANDLE};

    std::vector<VkSurfaceFormatKHR> preferred_surface_formats;
    std::vector<VkPresentModeKHR>   preferred_present_modes;
    VkQueue                         gfx_queue{VK_NULL_HANDLE};

    VkSurfaceTransformFlagsKHR transform_flags{0U};
};

struct acquire_next_image_details
{
    VkSemaphore   semaphore{VK_NULL_HANDLE};
    VkFence       fence{VK_NULL_HANDLE};
    std::uint32_t timeout{std::numeric_limits<std::uint32_t>::max()};
};

class RENDERING_EXPORT VulkanSwapchain
{
public:
    VulkanSwapchain(GLFWwindow* t_window);
    ~VulkanSwapchain();

    VulkanSwapchain(VulkanSwapchain const&)            = delete;
    VulkanSwapchain(VulkanSwapchain&&)                 = delete;
    VulkanSwapchain& operator=(VulkanSwapchain const&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&)      = delete;

    auto Create(swapchain_create_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    auto GetSurfaceFormat() const -> VkSurfaceFormatKHR const&;
    auto GetExtent() const -> VkExtent2D const&;
    auto GetScissor() const -> VkRect2D const&;
    auto GetViewport() const -> VkViewport const&;

    auto AcquireNextImage(acquire_next_image_details const& t_details) -> Result<std::uint32_t>;
    auto QueuePresent(std::uint32_t t_image_index, VkSemaphore t_wait_semaphore) const -> Result<void>;

    auto GetImage(std::uint32_t t_index) const -> VkImage;
    auto GetImageView(std::uint32_t t_index) const -> VkImageView;
private:
    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};

    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkDevice     m_device{VK_NULL_HANDLE};
    VkQueue      m_gfx_queue{VK_NULL_HANDLE};

    VkSurfaceFormatKHR m_surface_format;
    VkPresentModeKHR   m_present_mode;
    VkExtent2D         m_extent;
    VkRect2D           m_scissor;
    VkViewport         m_viewport;

    std::uint32_t            m_img_count{0U};
    std::vector<VkImage>     m_images;
    std::vector<VkImageView> m_image_views;

    GLFWwindow* m_window{nullptr};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_VULKAN_SWAPCHAIN_HPP_
