///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SCENE_MANAGER_HPP
#define POLOS_ECS_SCENE_MANAGER_HPP

#include "polos/polos_api.hpp"
#include "polos/utils/error_code.hpp"
#include "polos/utils/pair.hpp"
#include "polos/utils/string_id.hpp"

#include <filesystem>
#include <memory>
#include <vector>

namespace polos::core
{
class Engine;
}

namespace polos::ecs
{

class Scene;
struct SceneSnapshot;

class POLOS_API SceneManager
{
public:
    [[nodiscard]] static auto Get() -> SceneManager&;

    SceneManager();
    ~SceneManager();

    SceneManager(SceneManager const&)                    = delete;
    SceneManager(SceneManager&&)                         = delete;
    auto operator=(SceneManager const&) -> SceneManager& = delete;
    auto operator=(SceneManager&&) -> SceneManager&      = delete;

    auto CreateScene(std::string_view tName = "Scene") -> Scene*;
    auto MakeCurrentScene(utils::string_id tName) -> bool;
    auto GetCurrentScene() -> Scene*;

    auto QueryScene(utils::string_id tName) -> utils::Pair<Scene*, bool>;

    [[nodiscard]] auto        SaveCurrentScene() -> Result<bool>;
    [[nodiscard]] auto        TakeCurrentSceneSnapshot() -> Result<std::shared_ptr<SceneSnapshot const>>;
    [[nodiscard]] static auto SaveSceneSnapshot(SceneSnapshot const& tSnapshot) -> Result<bool>;
    auto                      LoadScene(std::string_view tScene) -> Scene*;
private:
    auto serializeEntity();

    friend class core::Engine;
    friend class SceneSerializationTest;

    static SceneManager* sInstance;
    Scene*               mCurrentScene{nullptr};

    std::vector<std::unique_ptr<Scene>> mScenes;
};
}// namespace polos::ecs

#endif// POLOS_ECS_SCENE_MANAGER_HPP
