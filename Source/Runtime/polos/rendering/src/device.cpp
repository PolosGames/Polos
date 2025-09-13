//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/device.hpp"

#include "polos/communication/event_bus.hpp"
#include "polos/logging/log_macros.hpp"

namespace polos::rendering
{

Device::Device()
{
    communication::Subscribe<communication::system_created>([this](communication::system_created& t_event) {
        on_system_created(t_event);
    });

    LogInfo("Device constructed.");
}

Device::~Device()
{
    LogInfo("Device destroyed.");
}

void Device::on_system_created(communication::system_created& t_event)
{
    LogInfo("System created {}", t_event);
}

}// namespace polos::rendering