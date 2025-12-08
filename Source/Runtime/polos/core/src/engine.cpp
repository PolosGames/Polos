///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/engine.hpp"

#include "polos/core/engine_layer.hpp"
#include "polos/core/main_loop.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/logging/logger.hpp"
#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/rendering_api.hpp"

#include <memory>

namespace polos::core
{

namespace
{

constexpr std::int32_t const kWidth{1280};
constexpr std::int32_t const kHeight{720};

}// namespace

std::int32_t Engine::Run(ILiveLayer* t_app_layer)
{
    std::unique_ptr<platform::PlatformManager> platform_manager = std::make_unique<platform::PlatformManager>();
    platform::PlatformManager::s_instance                       = platform_manager.get();

    if (!platform_manager->CreateNewWindow(kWidth, kHeight, "PolosEngine"))
    {
        LogCritical("Could not create engine window. Terminating!");
        return 2;
    }

    std::unique_ptr<rendering::RenderingApi> rendering_api =
        std::make_unique<rendering::RenderingApi>(platform_manager->GetMainWindow());
    rendering::RenderingApi::s_instance = rendering_api.get();

    std::unique_ptr<ILiveLayer> engine_layer{new EngineLayer{}};
    std::unique_ptr<ILiveLayer> app_layer{t_app_layer};
    engine_layer->Create();
    app_layer->Create();

    MainLoop loop{};// NOLINT
    loop.Run();

    app_layer->Destroy();
    engine_layer->Destroy();

    rendering_api->Shutdown();

    LogInfo("Polos exiting! Bye!");
    logging::FlushLogger(LOG_CTX_POLOS);

    return 0;
}

}// namespace polos::core
