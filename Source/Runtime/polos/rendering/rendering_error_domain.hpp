///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP
#define POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP

#include "polos/utils/error_code.hpp"
#include "polos/utils/error_domain.hpp"

#include <array>
#include <string_view>

namespace polos::rendering
{

// TODO(sorbatdev): Auto generate error codes and messages from a JSON or XML file
enum class RenderingErrc : utils::ErrorDomain::CodeType
{
    kGenericError,
    kFailedFindPhysDevice,
    kNoAdequateQueueFamily,
    kFailedCreateDevice,
    kNoAdequateExtension,
    kNoAdequateSurface,
    kFailedCreateSwapchainImageViews,
    kFailedCreateCmdPool,
    kFailedCmdBufAlloc,
    kFailedCreateSemaphore,
    kFailedCreateFence,
    kFailedCreateCurrFrameAsTexture,
    kFailedCreateRenderPass,
    kFailedCreateColorTarget,
    kFailedCreateDepthTarget,

    kFailedCreateInstance,
    kFailedCreateShaderModule,
    kFailedCreateDefaultTexture,

    kRenderingErrcCount,
};

class RenderingErrorDomain : public utils::ErrorDomain
{
public:
    using Errc = RenderingErrc;

    constexpr RenderingErrorDomain()
        : polos::utils::ErrorDomain(polos::utils::kRenderingErrorDomainId),
          mMessages({
              "An error has occured in the render pipeline!",
              "Failed to find a physical device! Exiting...",
              "No adequate queue family found! Exiting...",
              "Failed to create device instance! Exiting...",
              "No adequate extensions found on GPU that are needed for this instance! Exiting...",
              "No adequate surface found! Exiting...",
              "Failed to create image views for the swap chain images! Exiting...",
              "Failed to create a command pool! Exiting...",
              "Failed to allocate command buffers from the command pool! Exiting...",
              "Failed to create a semaphore! Exiting...",
              "Failed to create in-flight fence! Exiting...",
              "Failed to create current frame as texture!",
              "Failed to create render pass for a pass object! Exiting...",
              "Failed to create Color Target for the frame! Exiting...",
              "Failed to create Depth Target for the frame! Exiting...",
              "Failed to create the Vulkan instance!",
              "Failed to load or create a shader module!",
              "Failed to create the default white texture!",
          })
    {}

    virtual ~RenderingErrorDomain() = default;

    [[nodiscard]] constexpr auto Name() const -> std::string_view override
    { return "Rendering"; }

    [[nodiscard]] constexpr auto Message(CodeType tCode) const -> std::string_view override
    { return mMessages[static_cast<std::size_t>(tCode)]; }
private:
    std::array<std::string_view const, static_cast<std::size_t>(RenderingErrc::kRenderingErrcCount)> const mMessages;
};

namespace internal
{

constexpr RenderingErrorDomain g_error_domain;

}// namespace internal

constexpr utils::ErrorCode MakeErrorCode(RenderingErrorDomain::Errc tErr)
{ return utils::ErrorCode{static_cast<utils::ErrorDomain::CodeType>(tErr), internal::g_error_domain}; }

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP
