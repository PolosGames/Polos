///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_APP_BASE_LIVE_LAYER_HPP
#define POLOS_APP_BASE_LIVE_LAYER_HPP

#include "polos/app/i_live_layer.hpp"
#include "polos/polos_api.hpp"

namespace polos::app
{

class POLOS_API BaseLiveLayer : public ILiveLayer
{
public:
    BaseLiveLayer();
    ~BaseLiveLayer() override;

    auto Create() -> void override;
    auto Destroy() -> void override;

    [[nodiscard]] auto Name() const -> char const* override;
};

}// namespace polos::app

#endif// POLOS_APP_BASE_LIVE_LAYER_HPP
