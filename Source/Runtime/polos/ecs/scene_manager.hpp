///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SCENE_MANAGER_HPP
#define POLOS_ECS_SCENE_MANAGER_HPP

#include "polos/polos_api.hpp"

#include <memory>
#include <vector>

namespace polos::core
{
class Engine;
}

namespace polos::ecs
{

class Scene;

class POLOS_API SceneManager
{
public:
    SceneManager()  = default;
    ~SceneManager() = default;

    SceneManager(SceneManager const&)                    = delete;
    SceneManager(SceneManager&&)                         = delete;
    auto operator=(SceneManager const&) -> SceneManager& = delete;
    auto operator=(SceneManager&&) -> SceneManager&      = delete;

    static auto CreateScene() -> Scene*;
    static auto MakeCurrentScene(Scene* tScene) -> bool;
    static auto GetCurrentScene() -> Scene*;

    static auto LoadScene() -> Scene*;
private:
    friend class core::Engine;

    static SceneManager* sInstance;
    static Scene*        sCurrentScene;

    std::vector<std::unique_ptr<Scene>> mScenes;
};
}// namespace polos::ecs

#endif// POLOS_ECS_SCENE_MANAGER_HPP
