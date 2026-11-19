//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/rendering/vk/render_context.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{

RenderingApi* RenderingApi::sInstance{nullptr};

auto RenderingApi::Get() -> RenderingApi&
{
    return *sInstance;
}

RenderingApi::RenderingApi(platform::IWindowSurface& tSurface)
    : mWindowSurface{&tSurface}
{
    sInstance = this;

    createRenderContext();
}

RenderingApi::~RenderingApi()
{
    sInstance = nullptr;
}

auto RenderingApi::Shutdown() -> void
{
    mDebugDrawQueue.Clear();
    if (auto const result = mRenderContext->Shutdown(); !result.has_value())
    {
        LogWarn("{}", result.error());
    }
}

auto RenderingApi::BeginFrame() -> CommandBuffer*
{
    if (nullptr == mRenderContext)
    {
        mDebugDrawQueue.Clear();
        return nullptr;
    }
    auto* cmdBuf = mRenderContext->BeginFrame();
    if (cmdBuf == nullptr)
    {
        mDebugDrawQueue.Clear();
    }
    return cmdBuf;
}

auto RenderingApi::EndFrame() -> void
{
    if (nullptr != mRenderContext)
    {
        lastStatistics = mRenderContext->EndFrame();
    }
    mDebugDrawQueue.Clear();
}

auto RenderingApi::GetLastFrameStatistics() -> FrameStatistics const&
{
    return lastStatistics;
}

auto RenderingApi::GetDebugDrawQueue() -> DebugDrawQueue&
{
    return mDebugDrawQueue;
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
    PL_ASSERT(result.has_value(), "Could not initialize the Vulkan render context: {}", result.error().Message());
}

}// namespace polos::rendering
