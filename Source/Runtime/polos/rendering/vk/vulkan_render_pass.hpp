///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_VK_VULKAN_RENDER_PASS_HPP
#define POLOS_RENDERING_VK_VULKAN_RENDER_PASS_HPP

#include "polos/polos_api.hpp"

#include <vulkan/vulkan.h>

#include <span>

namespace polos::rendering
{

class CommandBuffer;

class POLOS_API VulkanRenderPass
{
public:
    VulkanRenderPass()                        = default;
    VulkanRenderPass(VulkanRenderPass const&) = delete;
    VulkanRenderPass(VulkanRenderPass&& tOther) noexcept;
    auto operator=(VulkanRenderPass const&) -> VulkanRenderPass& = delete;
    auto operator=(VulkanRenderPass&& tOther) noexcept -> VulkanRenderPass&;
    ~VulkanRenderPass();

    [[nodiscard]] auto GetHandle() const -> VkRenderPass;

    void Begin(
        CommandBuffer& tCmdBuf,
        VkFramebuffer                 tFramebuffer,
        VkRect2D const&               tRenderArea,
        std::span<VkClearValue const> tClearValues = {},
        VkSubpassContents             tContents    = VK_SUBPASS_CONTENTS_INLINE) const;
private:
    friend class RenderPassBuilder;

    VulkanRenderPass(VkDevice tDevice, VkRenderPass tHandle);

    VkDevice     mDevice{VK_NULL_HANDLE};
    VkRenderPass mHandle{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_VK_VULKAN_RENDER_PASS_HPP
