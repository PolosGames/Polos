//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "dummy_app.hpp"

#include <polos/communication/event_bus.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/logging/log_macros.hpp>

polos::core::ILiveLayer* GetLiveLayer(void* t_ptr)
{
    return nullptr == t_ptr ? new DummyApp{} : new (t_ptr) DummyApp{};
}

DummyApp::DummyApp()
{
    polos::communication::Subscribe<polos::communication::engine_update>(
        [this](polos::communication::engine_update& t_event)
        {
            OnEngineUpdate(t_event);
        });
}

DummyApp::~DummyApp() {}

char const* DummyApp::Name() const
{
    return "DummyApp";
}

void DummyApp::OnEngineUpdate(polos::communication::engine_update& t_event)
{
    LogInfo("{}: On Engine Update: [{}]", Name(), t_event);
}
