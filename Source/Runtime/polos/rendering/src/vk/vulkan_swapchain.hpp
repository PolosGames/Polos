///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_VK_VULKAN_SWAPCHAIN_HPP
#define POLOS_RENDERING_SRC_VK_VULKAN_SWAPCHAIN_HPP

#include "polos/communication/error_code.hpp"
#include "polos/polos_api.hpp"
#include "polos/rendering/interface/i_window_surface.hpp"

#include <vulkan/vulkan.h>

#include <limits>
#include <memory>
#include <vector>

namespace polos::rendering
{

class VulkanDevice;
class GpuImage;

struct alignas(128) SwapchainCreateDetails// NOLINT
{
    VulkanDevice const*   device;
    VkPhysicalDevice      physDevice{VK_NULL_HANDLE};
    VkSurfaceKHR          surface{VK_NULL_HANDLE};
    IWindowSurface const* windowSurface{nullptr};

    std::vector<VkSurfaceFormatKHR> preferredSurfaceFormats;
    std::vector<VkPresentModeKHR>   preferredPresentModes;
    VkQueue                         gfxQueue{VK_NULL_HANDLE};

    VkSurfaceTransformFlagsKHR transformFlags{0U};
};

struct alignas(32) AcquireNextImageDetails// NOLINT
{
    VkSemaphore   semaphore{VK_NULL_HANDLE};
    VkFence       fence{VK_NULL_HANDLE};
    std::uint64_t timeout{std::numeric_limits<std::uint64_t>::max()};
};

class VulkanSwapchain
{
public:
    VulkanSwapchain();
    ~VulkanSwapchain();

    VulkanSwapchain(VulkanSwapchain const&)            = delete;
    VulkanSwapchain(VulkanSwapchain&&)                 = delete;
    VulkanSwapchain& operator=(VulkanSwapchain const&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&)      = delete;

    auto Create(SwapchainCreateDetails const& tDetails) -> Result<void>;
    auto Destroy() -> Result<void>;

    [[nodiscard]] auto GetSurfaceFormat() const -> VkSurfaceFormatKHR const&;
    [[nodiscard]] auto GetExtent() const -> VkExtent2D const&;
    [[nodiscard]] auto GetExtent3D() const -> VkExtent3D const&;
    [[nodiscard]] auto GetScissor() const -> VkRect2D const&;
    [[nodiscard]] auto GetViewport() const -> VkViewport const&;

    auto AcquireNextImage(AcquireNextImageDetails const& tDetails) -> Result<std::uint32_t>;
    auto QueuePresent(VkSemaphore tWaitSemaphore) const -> Result<void>;

    [[nodiscard]] auto GetCurrentImage() const -> VkImage;
    [[nodiscard]] auto GetCurrentImageResource() const -> std::shared_ptr<GpuImage>;
    [[nodiscard]] auto GetCurrentImageIndex() const -> std::uint32_t;
    [[nodiscard]] auto GetCurrentImageView() const -> VkImageView;

    [[nodiscard]] auto GetImage(std::uint32_t tIndex) const -> VkImage;
    [[nodiscard]] auto GetImageView(std::uint32_t tIndex) const -> VkImageView;
    [[nodiscard]] auto GetImageCount() const -> std::uint32_t;
private:
    auto setupExtentAndViewport(VkPhysicalDevice tPhysDevice) -> void;
    auto selectFormatAndMode(
        VulkanDevice const*                    tDevice,
        std::vector<VkSurfaceFormatKHR> const& tFormats,
        std::vector<VkPresentModeKHR> const&   tModes) -> bool;
    auto createSwapchainHandle(VkSurfaceTransformFlagsKHR tTransformFlags) -> Result<void>;
    auto createImageViews() -> Result<void>;

    VkSwapchainKHR mSwapchain{VK_NULL_HANDLE};

    VkSurfaceKHR mSurface{VK_NULL_HANDLE};
    VkDevice     mDevice{VK_NULL_HANDLE};
    VkQueue      mGfxQueue{VK_NULL_HANDLE};

    VkSurfaceFormatKHR       mSurfaceFormat{};
    VkSurfaceCapabilitiesKHR mSurfaceCap{};
    VkPresentModeKHR         mPresentMode{};
    VkExtent2D               mExtent{};
    VkExtent3D               mExtent3D{};
    VkRect2D                 mScissor{};
    VkViewport               mViewport{};

    std::uint32_t                          mCurrentImage{0U};
    std::uint32_t                          mImgCount{0U};
    std::vector<VkImage>                   mImages;
    std::vector<std::shared_ptr<GpuImage>> mImageResources;
    std::vector<VkImageView>               mImageViews;

    IWindowSurface const* mWindowSurface{nullptr};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_VK_VULKAN_SWAPCHAIN_HPP
