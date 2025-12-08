///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_BASE_LIVE_LAYER_HPP
#define POLOS_CORE_BASE_LIVE_LAYER_HPP

#include "polos/core/i_live_layer.hpp"
#include "polos/core/module_macros.hpp"

namespace polos::core
{

class CORE_EXPORT BaseLiveLayer : public ILiveLayer
{
public:
    BaseLiveLayer();
    ~BaseLiveLayer() override;

    auto Create() -> void override;
    auto Destroy() -> void override;
private:
    [[nodiscard]] auto Name() const -> char const* override;
};

}// namespace polos::core

#endif// POLOS_CORE_BASE_LIVE_LAYER_HPP
