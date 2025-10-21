//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/module_operations.hpp"

#include "polos/communication/event_bus.hpp"
#include "polos/communication/module_reload.hpp"
#include "polos/rendering/shared_lib_out.hpp"

namespace polos::core
{

namespace
{
struct out_modules
{
    rendering::rendering_shared_lib_out rendering_module;
};

static out_modules modules;

}// namespace

auto LoadAllModules() -> bool
{
#if defined(HOT_RELOAD)
    return LoadRenderingModule() /* && LoadOtherModule() */;
#else
    return true;
#endif// HOT_RELOAD
}

auto UnloadAllModules() -> void
{
#if defined(HOT_RELOAD)
    rendering::UnloadRenderingModule(modules.rendering_module);
#endif// HOT_RELOAD
}

auto LoadRenderingModule() -> bool
{
#if defined(RENDERING_HOT_RELOAD)
    if (!rendering::LoadRenderingModule(modules.rendering_module))
    {
        return false;
    }

    LogInfo("-- Loaded module: {}", kRenderingLibName);

    communication::DispatchNow<communication::module_reload>("Rendering", modules.rendering_module.handle);
#endif// RENDERING_HOT_RELOAD

    return true;
}

auto GetRenderingModule() -> utils::base_shared_lib_out&
{
    return modules.rendering_module;
}

}// namespace polos::core
