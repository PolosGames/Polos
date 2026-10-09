///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef EXAMPLES_DUMMY_DUMMY_APP_HPP_
#define EXAMPLES_DUMMY_DUMMY_APP_HPP_

#include <polos/communication/key_release.hpp>
#include <polos/core/base_live_layer.hpp>
#include <polos/ecs/scene.hpp>

namespace polos::ecs
{
struct TransformComponent;
struct CameraComponent;
}// namespace polos::ecs

namespace polos::communication
{
struct EngineUpdate;
struct RenderUpdate;
}// namespace polos::communication

namespace polos::platform
{
struct FramebufferSize;
}// namespace polos::platform

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
    [[nodiscard]] static auto getWindowSize() -> polos::platform::FramebufferSize;

    auto onEngineUpdate(polos::communication::EngineUpdate& t_event) -> void;
    auto onRenderUpdate(polos::communication::RenderUpdate& t_event) -> void;

    // clang-format off
    auto moveCamera(polos::ecs::TransformComponent* tTransform,
                    float tDelta) -> void;
    // clang-format on

    polos::ecs::Scene* mScene;

    bool mUnloadInProgress{false};
};

}// namespace dummy_app

#endif// EXAMPLES_DUMMY_DUMMY_APP_HPP_
