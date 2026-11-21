///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/engine.hpp"

#include "polos/core/engine_layer.hpp"
#include "polos/core/main_loop.hpp"
#include "polos/ecs/system_registry.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/logging/logger.hpp"
#include "polos/platform/platform_manager.hpp"
#include "polos/rendering/rendering_api.hpp"
#include "polos/resource/resource_manager.hpp"
#include "polos/scene/scene_manager.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/build_info.hpp"
#include "polos/utils/profiling.hpp"

#include <format>
#include <memory>
#include <string>

namespace polos::core
{

namespace
{

constexpr std::int32_t const kWidth{1660};
constexpr std::int32_t const kHeight{1150};

}// namespace

std::int32_t Engine::Run(app::ILiveLayer* tAppLayer)
{
    std::string const build = utils::FormatBuildInfo(utils::GetBuildInfo());
    LogInfo("Polos build {}", build);

    auto platformManager = std::make_unique<platform::PlatformManager>();

    PL_ASSERT(platformManager->CreateNewWindow(kWidth, kHeight, tAppLayer->Name()), "Could not create the window.");

    auto& resources = resource::ResourceManager::Get();
    {
        POLOS_PROFILE_ZONE("Preload resource catalog");
        resources.LoadCatalog();
    }

    auto sceneManager   = std::make_unique<scene::SceneManager>();
    auto systemRegistry = std::make_unique<ecs::SystemRegistry>();
    auto renderingApi   = std::make_unique<rendering::RenderingApi>(platformManager->GetWindowSurface());

    std::unique_ptr<app::ILiveLayer> engineLayer{new EngineLayer{}};
    std::unique_ptr<app::ILiveLayer> appLayer{tAppLayer};
    engineLayer->Create();
    {
        POLOS_PROFILE_ZONE("Create application scene");
        appLayer->Create();
    }

    MainLoop{}.Run();// NOLINT

    appLayer->Destroy();
    engineLayer->Destroy();

    renderingApi->Shutdown();
    resources.ClearCache();

    logging::FlushLogger(LOG_CTX_APP);
    logging::FlushLogger(LOG_CTX_POLLY);
    logging::FlushLogger(LOG_CTX_POLOS);

    LogInfo("Polos exiting! Bye!");

    return 0;
}

}// namespace polos::core
