//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/engine/engine.hpp"
#include "polos/core/app/application.hpp"
#include "polos/logging/logger.hpp"

namespace polos::core
{

void Engine::Run()
{
    Application app;
    app.Run();
}

} // namespace polos::core
