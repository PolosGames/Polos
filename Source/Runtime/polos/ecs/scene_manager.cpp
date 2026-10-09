///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/scene_manager.hpp"

#include "polos/ecs/scene.hpp"
#include "polos/logging/log_macros.hpp"

namespace polos::ecs
{

SceneManager* SceneManager::sInstance     = nullptr;
Scene*        SceneManager::sCurrentScene = nullptr;

auto SceneManager::CreateScene() -> Scene*
{
    auto& scene = sInstance->mScenes.emplace_back(std::make_unique<Scene>());
    MakeCurrentScene(scene.get());
    return scene.get();
}

auto SceneManager::MakeCurrentScene(Scene* tScene) -> bool
{
    auto itr = std::ranges::find_if(sInstance->mScenes, [tScene](auto& tScenePtr) {
        return tScenePtr.get() == tScene;
    });
    if (itr == sInstance->mScenes.end())
    {
        LogError("Failed to find requested scene in scene library.");
        return false;
    }

    sCurrentScene = tScene;

    return true;
}

auto SceneManager::GetCurrentScene() -> Scene*
{
    return sCurrentScene;
}

}// namespace polos::ecs
