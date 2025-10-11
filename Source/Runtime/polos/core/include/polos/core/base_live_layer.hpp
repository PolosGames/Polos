//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_CORE_SRC_BASE_LIVE_LAYER_CPP_
#define POLOS_CORE_SRC_BASE_LIVE_LAYER_CPP_

#include "polos/core/i_live_layer.hpp"

#include "polos/core/module_macros.hpp"

namespace polos::core
{

class CORE_EXPORT BaseLiveLayer : public ILiveLayer
{
public:
    BaseLiveLayer();
    virtual ~BaseLiveLayer() override = default;
private:
    char const* Name() const override;
};

}// namespace polos::core


#endif// POLOS_CORE_SRC_BASE_LIVE_LAYER_CPP_
