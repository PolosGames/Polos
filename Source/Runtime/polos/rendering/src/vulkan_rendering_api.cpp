//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "scene/scene_data.hpp"
#include "vk/i_render_context.hpp"

#if defined(HOT_RELOAD)
#    include "vk/shared_lib_out.hpp"
#endif

namespace polos::rendering
{

RenderingApi* RenderingApi::sInstance{nullptr};

RenderingApi::RenderingApi(IWindowSurface& tSurface)
    : mWindowSurface{&tSurface}
{
#if defined(HOT_RELOAD)
    mRenderingModule = std::make_unique<RenderingSharedLibOut>();
#endif
    createRenderContext();

    mMainScene = std::make_shared<Scene>();
}

RenderingApi::~RenderingApi() = default;

auto RenderingApi::Shutdown() -> void
{
    if (auto const result = sInstance->mRenderContext->Shutdown(); !result.has_value())
    {
        LogWarn("{}", result.error());
    }
#if defined(HOT_RELOAD)
    UnloadRenderingModule(*mRenderingModule);
#endif// HOT_RELOAD
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
        SceneData const sceneData{
            .camera  = polos::rendering::RenderingApi::GetMainScene()->GetCamera(0U),
            .objects = polos::rendering::RenderingApi::GetMainScene()->GetObjects()
        };

        sInstance->mRenderContext->EndFrame(sceneData);
    }
}

auto RenderingApi::GetMainScene() -> std::shared_ptr<Scene>
{ return sInstance->mMainScene; }

#if defined(HOT_RELOAD)
auto RenderingApi::ReloadIfNeeded() -> bool
{
    if (!sInstance->mShouldReload)
    {
        return false;
    }

    sInstance->mShouldReload = false;
    sInstance->createRenderContext();

    LogInfo("Reloaded Render module.");

    return true;
}

auto RenderingApi::DispatchReload() -> void
{ sInstance->mShouldReload = true; }

auto RenderingApi::loadRenderingImplModule() -> bool
{
    LogInfo("Loading Rendering Module");

    UnloadRenderingModule(*mRenderingModule);
    if (!LoadRenderingModule(*mRenderingModule))
    {
        communication::DispatchNow<communication::EngineTerminate>();
        return false;
    }

    return true;
}
#endif// HOT_RELOAD

void RenderingApi::createRenderContext()
{
    if (nullptr != mRenderContext)
    {
        std::ignore = mRenderContext->Shutdown();
        mRenderContext.reset();
    }

#if defined(HOT_RELOAD)
    loadRenderingImplModule();
    mRenderContext = std::unique_ptr<IRenderContext>(mRenderingModule->createRenderContext());
#else
    mRenderContext = std::unique_ptr<IRenderContext>(CreateRenderContext());
#endif// HOT_RELOAD

    auto result = mRenderContext->Initialize(*mWindowSurface);
    if (!result.has_value())
    {
        LogCritical("RenderContext could not be initialized! {}", result.error().Message());
        communication::DispatchNow<communication::EngineTerminate>();
        return;
    }
}

}// namespace polos::rendering
