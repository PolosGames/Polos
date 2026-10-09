//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/ecs/scene_data.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{

RenderingApi* RenderingApi::sInstance{nullptr};

RenderingApi::RenderingApi(platform::IWindowSurface& tSurface)
    : mWindowSurface{&tSurface}
{ createRenderContext(); }

RenderingApi::~RenderingApi() = default;

auto RenderingApi::Shutdown() -> void
{
    if (auto const result = sInstance->mRenderContext->Shutdown(); !result.has_value())
    {
        LogWarn("{}", result.error());
    }
}

auto RenderingApi::BeginFrame() -> VkCommandBuffer
{
    if (nullptr == sInstance->mRenderContext)
    {
        return VK_NULL_HANDLE;
    }
    return sInstance->mRenderContext->BeginFrame();
}

auto RenderingApi::EndFrame() -> void
{
    if (nullptr != sInstance->mRenderContext)
    {
        sInstance->mRenderContext->EndFrame();
    }
}

void RenderingApi::createRenderContext()
{
    if (nullptr != mRenderContext)
    {
        std::ignore = mRenderContext->Shutdown();
        mRenderContext.reset();
    }

    mRenderContext = std::make_shared<RenderContext>();

    auto result = mRenderContext->Initialize(*mWindowSurface);
    PL_ASSERT(result.has_value(), "Could not initialize the Vulkan render context.");
}

}// namespace polos::rendering
