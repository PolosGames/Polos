///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef EXAMPLES_DUMMY_DUMMY_APP_HPP_
#define EXAMPLES_DUMMY_DUMMY_APP_HPP_

#include <polos/communication/key_release.hpp>
#include <polos/core/base_live_layer.hpp>

namespace polos::communication
{
struct engine_update;
struct render_update;
}// namespace polos::communication

namespace dummy_app
{

class DummyApp final : public polos::core::BaseLiveLayer
{
public:
    DummyApp();
    ~DummyApp() override;

    auto Create() -> void override;
    auto Destroy() -> void override;

    [[nodiscard]] auto Name() const -> char const* override;
private:
    auto onEngineUpdate(polos::communication::engine_update& t_event) -> void;
    auto onRenderUpdate(polos::communication::render_update& t_event) -> void;
    auto onKeyRelease(polos::communication::key_release t_event) -> void;

    bool m_unload_in_progress{false};
};

}// namespace dummy_app

#endif// EXAMPLES_DUMMY_DUMMY_APP_HPP_
