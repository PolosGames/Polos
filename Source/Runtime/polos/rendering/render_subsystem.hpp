///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_SUBSYSTEM_HPP
#define POLOS_RENDERING_RENDER_SUBSYSTEM_HPP

#include "polos/utils/error_code.hpp"

#include <concepts>
#include <cstddef>
#include <functional>
#include <span>
#include <type_traits>
#include <utility>

namespace polos::rendering
{

template<typename T>
concept RenderSubsystem = requires(T& tSubsystem, typename T::CreationDetails const& tDetails) {
    typename T::CreationDetails;
    { tSubsystem.Create(tDetails) } -> std::same_as<Result<void>>;
    { tSubsystem.Destroy() } -> std::same_as<void>;
};

struct SubsystemEntry
{
    std::move_only_function<Result<void>()> createFunc;
    std::move_only_function<void()>         destroyFunc;
};

/// The subsystem must outlive its entry. Details are constructed immediately before Create.
template<RenderSubsystem T, typename MakeDetails>
    requires std::invocable<MakeDetails&> &&
             std::same_as<std::invoke_result_t<MakeDetails&>, typename T::CreationDetails>
auto RegisterSubsystem(T& tSubsystem, MakeDetails tMakeDetails) -> SubsystemEntry
{
    return {
        .createFunc = [&tSubsystem, make_details = std::move(tMakeDetails)]() mutable -> Result<void> {
            auto const details = std::invoke(make_details);
            return tSubsystem.Create(details);
        },
        .destroyFunc =
            [&tSubsystem] {
                tSubsystem.Destroy();
            },
    };
}

inline auto DestroySubsystems(std::span<SubsystemEntry> tSubsystems, std::size_t& tInitializedCount) -> void
{
    while (tInitializedCount > 0U) { tSubsystems[--tInitializedCount].destroyFunc(); }
}

/// Destroy must also accept partially created or already destroyed state.
inline auto CreateSubsystems(std::span<SubsystemEntry> tSubsystems, std::size_t& tInitializedCount) -> Result<void>
{
    while (tInitializedCount < tSubsystems.size())
    {
        auto& subsystem = tSubsystems[tInitializedCount];
        if (auto result = subsystem.createFunc(); !result)
        {
            subsystem.destroyFunc();
            DestroySubsystems(tSubsystems, tInitializedCount);
            return result;
        }
        ++tInitializedCount;
    }
    return {};
}

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_SUBSYSTEM_HPP
