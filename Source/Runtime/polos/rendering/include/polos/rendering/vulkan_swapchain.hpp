///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VULKAN_SWAPCHAIN_HPP
#define POLOS_RENDERING_VULKAN_SWAPCHAIN_HPP

#include "polos/communication/error_code.hpp"
#include "polos/rendering/module_macros.hpp"

#include <vulkan/vulkan.h>

#include <limits>
#include <vector>

struct GLFWwindow;

namespace polos::rendering
{

class VulkanDevice;

struct alignas(128) swapchain_create_details// NOLINT
{
    VulkanDevice const* device;
    VkPhysicalDevice    phys_device{VK_NULL_HANDLE};
    VkSurfaceKHR        surface{VK_NULL_HANDLE};

    std::vector<VkSurfaceFormatKHR> preferred_surface_formats;
    std::vector<VkPresentModeKHR>   preferred_present_modes;
    VkQueue                         gfx_queue{VK_NULL_HANDLE};

    VkSurfaceTransformFlagsKHR transform_flags{0U};
};

struct alignas(32) acquire_next_image_details// NOLINT
{
    VkSemaphore   semaphore{VK_NULL_HANDLE};
    VkFence       fence{VK_NULL_HANDLE};
    std::uint64_t timeout{std::numeric_limits<std::uint64_t>::max()};
};

class VulkanSwapchain
{
public:
    explicit VulkanSwapchain(GLFWwindow* t_window);
    ~VulkanSwapchain();

    VulkanSwapchain(VulkanSwapchain const&)            = delete;
    VulkanSwapchain(VulkanSwapchain&&)                 = delete;
    VulkanSwapchain& operator=(VulkanSwapchain const&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&)      = delete;

    auto Create(swapchain_create_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    [[nodiscard]] auto GetSurfaceFormat() const -> VkSurfaceFormatKHR const&;
    [[nodiscard]] auto GetExtent() const -> VkExtent2D const&;
    [[nodiscard]] auto GetScissor() const -> VkRect2D const&;
    [[nodiscard]] auto GetViewport() const -> VkViewport const&;

    auto AcquireNextImage(acquire_next_image_details const& t_details) -> Result<std::uint32_t>;
    auto QueuePresent(VkSemaphore t_wait_semaphore) const -> Result<void>;

    [[nodiscard]] auto GetCurrentImage() const -> VkImage;
    [[nodiscard]] auto GetCurrentImageIndex() const -> std::uint32_t;
    [[nodiscard]] auto GetCurrentImageView() const -> VkImageView;

    [[nodiscard]] auto GetImage(std::uint32_t t_index) const -> VkImage;
    [[nodiscard]] auto GetImageView(std::uint32_t t_index) const -> VkImageView;
    [[nodiscard]] auto GetImageCount() const -> std::uint32_t;
private:
    auto setupExtentAndViewport(VkPhysicalDevice t_phys_device) -> void;
    auto selectFormatAndMode(
        VulkanDevice const*                    t_device,
        std::vector<VkSurfaceFormatKHR> const& t_formats,
        std::vector<VkPresentModeKHR> const&   t_modes) -> bool;
    auto createSwapchainHandle(VkSurfaceTransformFlagsKHR t_transform_flags) -> Result<void>;
    auto createImageViews() -> Result<void>;

    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};

    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkDevice     m_device{VK_NULL_HANDLE};
    VkQueue      m_gfx_queue{VK_NULL_HANDLE};

    VkSurfaceFormatKHR       m_surface_format{};
    VkSurfaceCapabilitiesKHR m_surface_cap{};
    VkPresentModeKHR         m_present_mode{};
    VkExtent2D               m_extent{};
    VkRect2D                 m_scissor{};
    VkViewport               m_viewport{};

    std::uint32_t            m_current_image{0U};
    std::uint32_t            m_img_count{0U};
    std::vector<VkImage>     m_images;
    std::vector<VkImageView> m_image_views;

    GLFWwindow* m_window{nullptr};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VULKAN_SWAPCHAIN_HPP
