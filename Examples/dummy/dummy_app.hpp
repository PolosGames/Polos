///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef EXAMPLES_DUMMY_DUMMY_APP_HPP_
#define EXAMPLES_DUMMY_DUMMY_APP_HPP_

#include <polos/communication/key_release.hpp>
#include <polos/core/base_live_layer.hpp>
#include <polos/rendering/camera3d.hpp>

namespace polos::communication
{
struct EngineUpdate;
struct RenderUpdate;
}// namespace polos::communication

namespace polos::rendering
{
struct FramebufferSize;
}// namespace polos::rendering

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
    [[nodiscard]] static auto getWindowSize() -> polos::rendering::FramebufferSize;

    auto onEngineUpdate(polos::communication::EngineUpdate& t_event) -> void;
    auto onRenderUpdate(polos::communication::RenderUpdate& t_event) -> void;
    auto moveCamera(polos::rendering::Camera3D* t_cam, std::float_t t_delta_time) -> void;

    std::size_t m_obj1{0U};
    std::size_t m_obj2{0U};

    bool m_unload_in_progress{false};
};

}// namespace dummy_app

#endif// EXAMPLES_DUMMY_DUMMY_APP_HPP_
