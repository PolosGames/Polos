//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/engine_layer.hpp"

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/rendering/rendering_api.hpp"

#include <GLFW/glfw3.h>

namespace polos::core
{

namespace
{

void OnKeyRelease(communication::key_release const& t_event)
{
    std::ignore = t_event;
#if defined(HOT_RELOAD)
    if (t_event.key == GLFW_KEY_R)
    {
        rendering::RenderingApi::DispatchReload();
    }
#endif// HOT_RELOAD
}

}// namespace

EngineLayer::EngineLayer()
{
    communication::Subscribe<communication::key_release>([](communication::key_release& t_event) {
        OnKeyRelease(t_event);
    });
}

EngineLayer::~EngineLayer() = default;

}// namespace polos::core
