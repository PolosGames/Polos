///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_SCENE_SCENE_GRAPH_HPP
#define POLOS_SCENE_SCENE_GRAPH_HPP

#include "polos/ecs/entity.hpp"

#include <cstdint>
#include <limits>
#include <vector>

namespace polos::scene
{
using SceneGraphHandle                                     = std::uint64_t;
inline constexpr SceneGraphHandle kInvalidSceneGraphHandle = std::numeric_limits<SceneGraphHandle>::max();

[[nodiscard]] inline constexpr auto
PackSceneGraphHandle(std::uint32_t const tIndex, std::uint32_t const tGeneration) noexcept -> SceneGraphHandle
{
    return (static_cast<SceneGraphHandle>(tGeneration) << 32U) | tIndex;
}

[[nodiscard]] inline constexpr auto GetSceneGraphIndex(SceneGraphHandle const tHandle) noexcept -> std::uint32_t
{
    return static_cast<std::uint32_t>(tHandle);
}

[[nodiscard]] inline constexpr auto GetSceneGraphGeneration(SceneGraphHandle const tHandle) noexcept -> std::uint32_t
{
    return static_cast<std::uint32_t>(tHandle >> 32U);
}

class POLOS_API SceneGraph
{
public:
    explicit SceneGraph(ecs::EntityHandle tRoot = ecs::EntityHandle{.index = 0U, .version = 0U});
    ~SceneGraph() = default;

    /// @brief Prepends a child
    /// @note sibling traversal visits newest children first
    [[nodiscard]] auto AddChild(SceneGraphHandle tParent, ecs::EntityHandle tEntity) -> SceneGraphHandle;

    [[nodiscard]] auto GetParent(SceneGraphHandle tChild) const -> ecs::EntityHandle;

    /// @brief Returns the first child.
    /// @note Follow GetNextSibling to visit the remaining children.
    [[nodiscard]] auto GetChild(SceneGraphHandle tParent) const -> ecs::EntityHandle;
    [[nodiscard]] auto GetNextSibling(SceneGraphHandle tChild) const -> ecs::EntityHandle;

    /// @brief Removes a subtree; Scene must retire its entities too. The root cannot be retired.
    auto RetireNode(SceneGraphHandle tHandle) -> std::uint32_t;

    /// @brief Removes the first child's subtree, if present.
    auto RetireChild(SceneGraphHandle tParent) -> bool;

    /// @brief Removes all descendant nodes, retaining the parent. Returns the number removed.
    auto RetireAllChildren(SceneGraphHandle tParent) -> std::uint32_t;
private:
    friend class Scene;
    friend class SceneManager;

    using Index                          = std::uint32_t;
    static constexpr Index kInvalidIndex = ecs::kInvalidEntityIndex;

    struct alignas(32) Node
    {
        ecs::EntityHandle entity;
        Index         parent{kInvalidIndex};
        Index         leftChild{kInvalidIndex};
        Index         rightSibling{kInvalidIndex};
        Index         previousSibling{kInvalidIndex};
        std::uint32_t generation{0U};
        int32_t       layer{-1};// sceneRoot is an invalid layer
    };

    [[nodiscard]] auto isValid(SceneGraphHandle tHandle) const -> bool;
    auto               detach(Index tIndex) -> void;
    auto               retireSubtree(Index tIndex) -> std::uint32_t;

    // Retired nodes reuse parent as the free-list link.
    Index             mFreeHead{kInvalidIndex};
    std::vector<Node> mNodes;

    std::vector<uint32_t> mObjectCountByLayer;
};
}// namespace polos::scene

#endif// POLOS_SCENE_SCENE_GRAPH_HPP
