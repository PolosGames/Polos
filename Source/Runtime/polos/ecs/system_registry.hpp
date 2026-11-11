///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_SYSTEM_REGISTRY_HPP
#define POLOS_ECS_SYSTEM_REGISTRY_HPP

#include <functional>
#include <utility>
#include <vector>

namespace polos::ecs
{
class Scene;

class SystemRegistry
{
public:
    using SystemUpdate = std::function<void(Scene*, float)>;

    /// Register systems before calling UpdateAll.
    void Register(SystemUpdate tUpdate)
    {
        mSystems.push_back(std::move(tUpdate));
    }

    void UpdateAll(Scene* tScene, float tDeltaTime) const
    {
        for (auto const& update : mSystems)
        {
            update(tScene, tDeltaTime);
        }
    }
private:
    std::vector<SystemUpdate> mSystems;
};
}// namespace polos::ecs

#endif// POLOS_ECS_SYSTEM_REGISTRY_HPP
