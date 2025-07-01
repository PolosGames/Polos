///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/core/engine.hpp"

#include "polos/core/main_loop.hpp"
#include "polos/logging/logger.hpp"
#include "polos/platform/window_manager.hpp"

#include <memory>

namespace polos::core
{

std::int32_t Engine::Run(ILiveLayer* t_app_layer)
{
    std::unique_ptr<ILiveLayer> app_layer{t_app_layer};

    platform::WindowManager::Instance();

    MainLoop loop{};
    loop.Run();

    return 0;
}

}// namespace polos::core
