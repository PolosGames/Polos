//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/core/base_live_layer.hpp"

#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/event_bus.hpp"

#include <csignal>

namespace polos::core
{

BaseLiveLayer::BaseLiveLayer()  = default;
BaseLiveLayer::~BaseLiveLayer() = default;

void BaseLiveLayer::Create() {}

void BaseLiveLayer::Destroy() {}

char const* BaseLiveLayer::Name() const
{
    return "BaseLiveLayer";
}

}// namespace polos::core