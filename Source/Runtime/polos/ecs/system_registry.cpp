///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/system_registry.hpp"

namespace polos::ecs
{

SystemRegistry* SystemRegistry::sInstance = nullptr;

SystemRegistry::SystemRegistry()
{
    sInstance = this;
}

SystemRegistry::~SystemRegistry()
{
    sInstance = nullptr;
}

auto SystemRegistry::Get() -> SystemRegistry&
{
    return *sInstance;
}

void SystemRegistry::Register(SystemUpdate&& tUpdate)
{
    mSystems.push_back(std::move(tUpdate));
}

void SystemRegistry::UpdateAll(scene::Scene* tScene, float tDeltaTime)
{
    for (auto& update : mSystems)
    {
        std::invoke(update, tScene, tDeltaTime);
    }
}


}// namespace polos::ecs
