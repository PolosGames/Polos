///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SYSTEM_REGISTRY_HPP
#define POLOS_ECS_SYSTEM_REGISTRY_HPP

#include <functional>
#include <utility>
#include <vector>

namespace polos::scene
{
class Scene;
}// namespace polos::scene

namespace polos::ecs
{
class SystemRegistry
{
public:
    using SystemUpdate = std::move_only_function<void(scene::Scene*, float)>;

    SystemRegistry();
    ~SystemRegistry();

    [[nodiscard]] static auto Get() -> SystemRegistry&;

    void Register(SystemUpdate&& tUpdate);
    void UpdateAll(scene::Scene* tScene, float tDeltaTime);
private:
    static SystemRegistry* sInstance;

    std::vector<SystemUpdate> mSystems;
};
}// namespace polos::ecs

#endif// POLOS_ECS_SYSTEM_REGISTRY_HPP
