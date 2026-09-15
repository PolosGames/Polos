///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/engine.hpp"

#include "engine_layer.hpp"
#include "main_loop.hpp"
#include "polos/core/input_state.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/logging/logger.hpp"
#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/utils/build_info.hpp"

#include <format>
#include <memory>
#include <string>

namespace polos::core
{

namespace
{

constexpr std::int32_t const kWidth{1280};
constexpr std::int32_t const kHeight{720};

}// namespace

std::int32_t Engine::Run(ILiveLayer* tAppLayer)
{
    std::string const build = utils::FormatBuildInfo(utils::GetBuildInfo());
    LogInfo("Polos build {}", build);

    std::unique_ptr<platform::PlatformManager> platform_manager = std::make_unique<platform::PlatformManager>();
    platform::PlatformManager::sInstance                        = platform_manager.get();

    if (!platform_manager->CreateNewWindow(kWidth, kHeight, std::format("PolosEngine - {}", build)))
    {
        LogCritical("Could not create engine window. Terminating!");
        return 2;
    }

    std::unique_ptr<rendering::RenderingApi> rendering_api =
        std::make_unique<rendering::RenderingApi>(platform_manager->GetWindowSurface());
    rendering::RenderingApi::sInstance = rendering_api.get();

    std::unique_ptr<ILiveLayer> engine_layer{new EngineLayer{}};
    std::unique_ptr<ILiveLayer> app_layer{tAppLayer};
    engine_layer->Create();
    app_layer->Create();

    MainLoop loop{};// NOLINT
    loop.Run();

    app_layer->Destroy();
    engine_layer->Destroy();

    rendering_api->Shutdown();

    logging::FlushLogger(LOG_CTX_APP);
    logging::FlushLogger(LOG_CTX_POLLY);
    logging::FlushLogger(LOG_CTX_POLOS);

    LogInfo("Polos exiting! Bye!");

    return 0;
}

}// namespace polos::core
