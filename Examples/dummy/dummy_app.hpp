//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include <polos/communication/engine_update.hpp>
#include <polos/core/i_live_layer.hpp>

#ifndef EXAMPLES_DUMMY_DUMMY_APP_HPP_
#    define EXAMPLES_DUMMY_DUMMY_APP_HPP_

class DummyApp : public polos::core::ILiveLayer
{
public:
    DummyApp();
    ~DummyApp() override;

    char const* Name() const override;
private:
    void OnEngineUpdate(polos::communication::engine_update& t_event);
};

#endif// EXAMPLES_DUMMY_DUMMY_APP_HPP_
