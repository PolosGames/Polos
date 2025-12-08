///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP
#define POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP

#include "polos/communication/error_code.hpp"
#include "polos/communication/error_domain.hpp"

#include <array>
#include <string_view>

namespace polos::rendering
{

// TODO(sorbatdev): Auto generate error codes and messages from a JSON or XML file
enum class RenderingErrc : communication::ErrorDomain::CodeType
{
    kFailedCreateInstance,
    kFailedCreateSurface,
    kFailedFindPhysDevice,
    kNoAdequatePhysDevice,
    kNoAdequateQueueFamily,
    kFailedCreateDevice,
    kFailedCreateSwapchain,
    kNoAdequateSurface,
    kFailedCreateSwapchainImageViews,
    kFailedCreateShaderModule,
    kFailedCreatePipelineLayout,
    kFailedCreatePipeline,
    kFailedPresentQueue,
    kFailedCreateCmdPool,
    kFailedCmdBufAlloc,
    kFailedCreateSemaphore,
    kFailedCreateFence,
    kFailedCreateCurrFrameAsTexture,
    kFailedCreateRenderPass,
    kFailedFindPipeline,
    kFailedAcquireNextImage,

    kRenderingErrcCount,
};

class RenderingErrorDomain : public communication::ErrorDomain
{
public:
    using Errc = RenderingErrc;

    constexpr RenderingErrorDomain()
        : polos::communication::ErrorDomain(polos::communication::kRenderingErrorDomainId),
          m_messages({
              "Failed to create Vulkan instance! Exiting...",
              "Failed to create surface! Where are we going to draw? Exiting...",
              "Failed to find a physical device! Exiting...",
              "No adequate physical device on host! Exiting...",
              "No adequate queue family found! Exiting...",
              "Failed to create device instance! Exiting...",
              "Failed to create the swapchain! Exiting...",
              "No adequate surface found! Exiting...",
              "Failed to create image views for the swap chain images! Exiting...",
              "Failed to create shader module! Exiting...",
              "Failed to create a pipeline layout! Exiting...",
              "Failed to create pipeline object! Exiting...",
              "Failed to present the image to the presentation queue!",
              "Failed to create a command pool! Exiting...",
              "Failed to allocate command buffers from the command pool! Exiting...",
              "Failed to create a semaphore! Exiting...",
              "Failed to create in-flight fence! Exiting...",
              "Failed to create current frame as texture!",
              "Failed to create render pass for a pass object! Exiting...",
              "Failed to find pipeline in cache! Please create it first.",
              "Failed to acquire next image from the swapchain! Recreating swapchain...",
          })
    {}

    virtual ~RenderingErrorDomain() = default;

    [[nodiscard]] constexpr auto Name() const -> std::string_view override
    {
        return "Rendering";
    }

    [[nodiscard]] constexpr auto Message(CodeType t_code) const -> std::string_view override
    {
        return m_messages[static_cast<std::size_t>(t_code)];
    }
private:
    std::array<std::string_view const, static_cast<std::size_t>(RenderingErrc::kRenderingErrcCount)> const m_messages;
};

namespace internal
{

constexpr RenderingErrorDomain g_error_domain;

}// namespace internal

constexpr communication::ErrorCode MakeErrorCode(RenderingErrorDomain::Errc t_err)
{
    return communication::ErrorCode{static_cast<communication::ErrorDomain::CodeType>(t_err), internal::g_error_domain};
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP
