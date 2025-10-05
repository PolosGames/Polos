//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/communication/error_domain.hpp"

#include <array>

namespace polos::rendering
{

// TODO: Auto generate error codes and messages from a JSON or XML file
enum class RenderingErrc : communication::ErrorDomain::CodeType
{
    kInstanceNotCreated             = 0U,
    kSurfaceNotCreated              = 1U,
    kNoPhysicalDeviceOnHost         = 2U,
    kNoAdequatePhysicalDeviceFound  = 3U,
    kPreferableQueueFamilyNotFound  = 4U,
    kFailedDeviceCreation           = 5U,
    kSwapchainNotCreated            = 6U,
    kNoAdequateSurface              = 7U,
    kSwapchainImageViewCreationFail = 8U,
    kShaderModuleNotCreated         = 9U,
    kPipelineNotFoundInCache        = 10U,
    kFailedToCreatePipelineLayout   = 11U,
    kFailedToCreatePipeline         = 12U,
    kFailedToAcquireSwapchainImage  = 13U,
    kFailedToPresentQueue           = 14U,
    kErrorDestroyingRenderSubmodule = 15U,

    kRenderingErrcCount,
};

class RenderingErrorDomain : public communication::ErrorDomain
{
public:
    using Errc = RenderingErrc;

    constexpr RenderingErrorDomain()
        : polos::communication::ErrorDomain(0x07U),
          m_messages({
              "Vulkan instance could not be created! SERIOUS SHIT",
              "Could not create a window surface! Where are we going to draw?",
              "Could not find any physical devices on machine... I hope this toaster makes good toasts.",
              "No adequate physical device found on machine! Come on...",
              "Could not find a suitable queue family on physical device!",
              "Logical device creation for has failed!",
              "Could not create a swapchain for the window surface!",
              "No adequate surface found for swapchain.",
              "Could not create image views for the swap chain images!",
              "Could not create a shader module!",
              "Requested pipeline was not found in pipeline cache. Create it with LoadOrCreatePipeline!",
              "Could not create a pipeline layout!",
              "Could not create VkPipeline object!",
              "Could not acquire next image from the swapchain!",
              "Could not present the image to the presentation queue!",
              "Error occurred while destroying a rendering submodule!",
          })
    {}

    virtual ~RenderingErrorDomain() = default;

    constexpr std::string_view Name() const override
    {
        return "Rendering";
    }

    constexpr std::string_view Message(CodeType t_code) const override
    {
        return m_messages[static_cast<std::size_t>(t_code)];
    }
private:
    std::array<std::string_view const, static_cast<std::size_t>(RenderingErrc::kRenderingErrcCount)> const m_messages;
};

namespace internal
{
inline constexpr RenderingErrorDomain g_error_domain;
}

constexpr communication::ErrorCode MakeErrorCode(RenderingErrorDomain::Errc t_err)
{
    return communication::ErrorCode{static_cast<communication::ErrorDomain::CodeType>(t_err), internal::g_error_domain};
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERING_ERROR_DOMAIN_HPP_
