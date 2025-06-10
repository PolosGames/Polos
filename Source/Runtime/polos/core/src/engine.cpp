//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/engine.hpp"
#include "polos/core/main_loop.hpp"
#include "polos/logging/logger.hpp"

#include <memory>

namespace polos::core
{

void Engine::Run(ILiveLayer* t_app_layer)
{
    MainLoop loop{};
    loop.Run();

    delete t_app_layer;
}

}// namespace polos::core
