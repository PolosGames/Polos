///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/scene/scene_graph.hpp"

#include <utility>

namespace polos::scene
{

SceneGraph::SceneGraph(ecs::EntityHandle const tRoot)
{
    mNodes.reserve(ecs::Settings::kMaxEntities);
    mNodes.push_back(Node{.entity = tRoot});
}

auto SceneGraph::AddChild(SceneGraphHandle const tParent, ecs::EntityHandle const tEntity) -> SceneGraphHandle
{
    if (!isValid(tParent) || tEntity.index >= ecs::kInvalidEntityIndex || tEntity.version >= ecs::kInvalidEntityVersion)
    {
        return kInvalidSceneGraphHandle;
    }

    Index index{kInvalidIndex};
    if (mFreeHead != kInvalidIndex)
    {
        index                 = mFreeHead;
        mFreeHead             = mNodes[index].parent;
        auto const generation = mNodes[index].generation;
        mNodes[index]         = Node{.entity = tEntity, .generation = generation};
    }
    else
    {
        if (mNodes.size() == ecs::Settings::kMaxEntities)
        {
            return kInvalidSceneGraphHandle;
        }
        index = static_cast<Index>(mNodes.size());
        mNodes.push_back(Node{.entity = tEntity});
    }

    auto const parent = GetSceneGraphIndex(tParent);
    auto&      node   = mNodes[index];
    node.parent       = parent;
    node.rightSibling = mNodes[parent].leftChild;
    if (node.rightSibling != kInvalidIndex)
    {
        mNodes[node.rightSibling].previousSibling = index;
    }
    mNodes[parent].leftChild = index;

    node.layer = 1 + mNodes[parent].layer;

    if (std::cmp_equal(node.layer, mObjectCountByLayer.size()))
    {
        mObjectCountByLayer.push_back(1);
    }
    else
    {
        ++mObjectCountByLayer[static_cast<size_t>(node.layer)];
    }

    return PackSceneGraphHandle(index, mNodes[index].generation);
}

auto SceneGraph::GetParent(SceneGraphHandle const tChild) const -> ecs::EntityHandle
{
    if (!isValid(tChild))
    {
        return ecs::kInvalidEntityHandle;
    }
    auto const parent = mNodes[GetSceneGraphIndex(tChild)].parent;
    return parent != kInvalidIndex ? mNodes[parent].entity : ecs::kInvalidEntityHandle;
}

auto SceneGraph::GetChild(SceneGraphHandle const tParent) const -> ecs::EntityHandle
{
    if (!isValid(tParent))
    {
        return ecs::kInvalidEntityHandle;
    }
    auto const child = mNodes[GetSceneGraphIndex(tParent)].leftChild;
    return child != kInvalidIndex ? mNodes[child].entity : ecs::kInvalidEntityHandle;
}

auto SceneGraph::GetNextSibling(SceneGraphHandle const tChild) const -> ecs::EntityHandle
{
    if (!isValid(tChild))
    {
        return ecs::kInvalidEntityHandle;
    }

    auto const sibling = mNodes[GetSceneGraphIndex(tChild)].rightSibling;
    return sibling != kInvalidIndex ? mNodes[sibling].entity : ecs::kInvalidEntityHandle;
}

auto SceneGraph::RetireNode(SceneGraphHandle const tHandle) -> std::uint32_t
{
    return isValid(tHandle) && GetSceneGraphIndex(tHandle) != 0U ? retireSubtree(GetSceneGraphIndex(tHandle)) : 0U;
}

auto SceneGraph::RetireChild(SceneGraphHandle const tParent) -> bool
{
    if (!isValid(tParent))
    {
        return false;
    }

    auto const child = mNodes[GetSceneGraphIndex(tParent)].leftChild;
    if (child == kInvalidIndex)
    {
        return false;
    }

    retireSubtree(child);
    return true;
}

auto SceneGraph::RetireAllChildren(SceneGraphHandle const tParent) -> std::uint32_t
{
    if (!isValid(tParent))
    {
        return 0U;
    }

    std::uint32_t count{0U};
    auto const    parent = GetSceneGraphIndex(tParent);

    while (mNodes[parent].leftChild != kInvalidIndex)
    {
        count += retireSubtree(mNodes[parent].leftChild);
    }

    return count;
}

auto SceneGraph::isValid(SceneGraphHandle const tHandle) const -> bool
{
    auto const index = GetSceneGraphIndex(tHandle);

    return index < mNodes.size() && mNodes[index].entity.index != ecs::kInvalidEntityIndex &&
           mNodes[index].generation == GetSceneGraphGeneration(tHandle);
}

auto SceneGraph::detach(Index const tIndex) -> void
{
    auto& node = mNodes[tIndex];
    if (node.previousSibling == kInvalidIndex)
    {
        mNodes[node.parent].leftChild = node.rightSibling;
    }
    else
    {
        mNodes[node.previousSibling].rightSibling = node.rightSibling;
    }
    if (node.rightSibling != kInvalidIndex)
    {
        mNodes[node.rightSibling].previousSibling = node.previousSibling;
    }
    node.rightSibling = kInvalidIndex;
}

auto SceneGraph::retireSubtree(Index const tIndex) -> std::uint32_t
{
    detach(tIndex);
    std::uint32_t count   = 0U;
    auto          current = tIndex;
    while (true)
    {
        auto& node = mNodes[current];

        if (node.leftChild != kInvalidIndex)
        {
            auto const child = node.leftChild;
            node.leftChild   = kInvalidIndex;
            current          = child;
            continue;
        }

        auto const next = node.rightSibling != kInvalidIndex ? node.rightSibling : node.parent;
        node.entity     = ecs::kInvalidEntityHandle;
        --mObjectCountByLayer[static_cast<std::size_t>(node.layer)];

        // Exhausted generations stay retired so an old handle can never become valid again.
        if (node.generation != std::numeric_limits<std::uint32_t>::max())
        {
            ++node.generation;
            node.parent = mFreeHead;
            mFreeHead   = current;
        }
        ++count;
        if (current == tIndex)
        {
            return count;
        }
        current = next;
    }
}

}// namespace polos::scene
