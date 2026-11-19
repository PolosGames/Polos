///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef CATCHTHECOIN_GAME_HPP
#define CATCHTHECOIN_GAME_HPP

#include "polos/ecs/entity.hpp"
namespace polos::ecs
{
class Scene;
}// namespace polos::ecs

namespace ctc
{

class Game
{
public:
    void Setup(polos::ecs::Scene* tScene);

    void Update(float tDt);
    void Render(float tDt);
private:
    void renderEditor(float tDt);

    polos::ecs::Scene* mScene;
    bool               mShowDemo{false};

    polos::ecs::EntityHandle mCharacterCam;
};

}// namespace ctc

#endif// CATCHTHECOIN_GAME_HPP
