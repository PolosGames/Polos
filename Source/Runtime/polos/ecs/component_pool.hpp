///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_ECS_COMPONENT_POOL_HPP
#define POLOS_ECS_COMPONENT_POOL_HPP

#include "polos/ecs/component.hpp"
#include "polos/ecs/settings.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/polos_api.hpp"

#include <cstdlib>

namespace polos::ecs
{

/// @brief Represents a single component's pool. Determined by the ComponentType
/// Initialized with Create<T>() and destroyed with Destroy<T>().
class POLOS_API ComponentPool
{
public:
    ComponentPool() = default;
    ~ComponentPool();
    ComponentPool(ComponentPool&& tOther);
    ComponentPool& operator=(ComponentPool&& tOther);

    ComponentPool(ComponentPool const&)            = delete;
    ComponentPool& operator=(ComponentPool const&) = delete;

    template<typename T>
    auto Create() -> void
    {
        if (mData != nullptr)
        {
            LogError("Component Pool already created for component type {}", static_cast<std::uint32_t>(T::kType));
            return;
        }

        mData    = new T[Settings::kMaxEntities]();
        mDestroy = [](void* tPtr) {
            delete[] static_cast<T*>(tPtr);
        };
        elemSize = sizeof(T);

        mInitialized = true;
    }

    auto Destroy() -> void;

    template<typename T>
    T* Get(std::size_t tIndex)
    {
        return &static_cast<T*>(mData)[tIndex];
    }

    [[nodiscard]] auto IsInitialized() const -> bool;
    [[nodiscard]] auto GetTotalComponents() const -> std::uint32_t;

    ComponentType type{ComponentType::kVisibility};
    std::size_t   elemSize{0U};
private:
    friend class Scene;
    friend class SceneManager;

    std::uint32_t mTotalComponents{0U};

    void* mData{nullptr};
    void (*mDestroy)(void*) = nullptr;
    bool mInitialized{false};
};

}// namespace polos::ecs

#endif// POLOS_ECS_COMPONENT_POOL_HPP
