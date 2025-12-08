///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_ENGINE_LAYER_HPP
#define POLOS_CORE_ENGINE_LAYER_HPP

#include "polos/communication/key_release.hpp"
#include "polos/core/base_live_layer.hpp"

namespace polos::core
{

/// @brief Layer that should handle all engine related events and relays needed outputs.
///
/// This class will also do all module loading operations under the hood and emit module_reload event.
class EngineLayer : public BaseLiveLayer
{
public:
    EngineLayer();
    ~EngineLayer() override;
};

}// namespace polos::core

#endif// POLOS_CORE_ENGINE_LAYER_HPP
