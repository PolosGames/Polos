///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/engine.hpp"

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/core/engine_layer.hpp"
#include "polos/core/main_loop.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/logging/logger.hpp"
#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/render_context.hpp"

#include <memory>

namespace polos::core
{

std::int32_t Engine::Run(ILiveLayer* t_app_layer)
{
    std::unique_ptr<platform::PlatformManager> platform_manager = std::make_unique<platform::PlatformManager>();
    platform_manager->s_instance                                = platform_manager.get();

    if (!platform_manager->CreateNewWindow(1280, 720, "PolosEngine"))
    {
        LogCritical("Could not create engine window. Terminating!");
        return 2;
    }

    std::unique_ptr<ILiveLayer> engine_layer{new EngineLayer{}};
    std::unique_ptr<ILiveLayer> app_layer{t_app_layer};
    engine_layer->Create();
    app_layer->Create();

    MainLoop loop{};
    loop.Run();

    app_layer->Destroy();
    engine_layer->Destroy();

    LogInfo("Polos exiting! Bye!");
    logging::FlushLogger(LOG_CTX_POLOS);

    return 0;
}

}// namespace polos::core
