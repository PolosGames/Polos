///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "game.hpp"

#include <polos/app/base_live_layer.hpp>
#include <polos/app/polos_main.hpp>
#include <polos/communication/event_bus.hpp>
#include <polos/core/events/engine_update.hpp>
#include <polos/rendering/events/render_update.hpp>
#include <polos/scene/scene.hpp>
#include <polos/scene/scene_manager.hpp>

namespace ctc
{
class Tetris : public polos::app::BaseLiveLayer
{
public:
    void Create() override
    {
        PL_SUBSCRIBE_TO_EVENT_MEMBER_FN(polos::core::EngineUpdate, Update);
        PL_SUBSCRIBE_TO_EVENT_MEMBER_FN(polos::rendering::RenderUpdate, Render);

        auto* scene = polos::scene::SceneManager::Get().CreateScene("Tetris", false);

        game.Setup(scene);
    }

    void Destroy() override
    {}

    [[nodiscard]] auto Name() const -> char const* override
    {
        return "Tetris";
    }

    void Update(polos::core::EngineUpdate& tEvent)
    {
        game.Update(tEvent.deltaTime);
    }

    void Render(polos::rendering::RenderUpdate& tEvent)
    {
        game.Render(tEvent.deltaTime);
    }

    Game game;
};
};// namespace ctc

namespace polos::app
{
auto CreateApplication(int tArgc, char** tArgv) -> ILiveLayer*
{
    return new ctc::Tetris();
}
}// namespace polos::app
