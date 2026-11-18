///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "game.hpp"

#include <polos/communication/engine_update.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/communication/render_update.hpp>
#include <polos/core/base_live_layer.hpp>
#include <polos/core/polos_main.hpp>
#include <polos/ecs/scene.hpp>
#include <polos/ecs/scene_manager.hpp>

namespace ctc
{
class CatchTheCoin : public polos::core::BaseLiveLayer
{
public:
    void Create() override
    {
        PL_SUBSCRIBE_TO_EVENT_MEMBER_FN(polos::communication::EngineUpdate, Update);
        PL_SUBSCRIBE_TO_EVENT_MEMBER_FN(polos::communication::RenderUpdate, Render);

        namespace ecs = polos::ecs;
        auto* scene   = ecs::SceneManager::Get().CreateScene();

        game.Setup(scene);
    }

    void Destroy() override
    {}

    [[nodiscard]] auto Name() const -> char const* override
    {
        return "CatchTheCoin";
    }

    void Update(polos::communication::EngineUpdate& tEvent)
    {
        game.Update(tEvent.deltaTime);
    }

    void Render(polos::communication::RenderUpdate& tEvent)
    {
        game.Render(tEvent.deltaTime);
    }

    Game game;
};
};// namespace ctc

namespace polos
{
auto CreateApplication(int tArgc, char** tArgv) -> polos::core::ILiveLayer*
{
    return new ctc::CatchTheCoin();
}
}// namespace polos
