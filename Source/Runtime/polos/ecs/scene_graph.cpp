///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/scene_graph.hpp"

namespace polos::ecs
{

SceneGraph::SceneGraph(EntityHandle const tRoot)
{
    mNodes.reserve(Settings::kMaxEntities);
    mNodes.push_back(Node{.entity = tRoot});
}

auto SceneGraph::AddChild(SceneGraphHandle const tParent, EntityHandle const tEntity) -> SceneGraphHandle
{
    if (!isValid(tParent) || tEntity.index >= kInvalidEntityIndex || tEntity.version >= kInvalidEntityVersion)
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
        if (mNodes.size() == Settings::kMaxEntities)
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

    return PackSceneGraphHandle(index, mNodes[index].generation);
}

auto SceneGraph::GetParent(SceneGraphHandle const tChild) const -> EntityHandle
{
    if (!isValid(tChild))
    {
        return kInvalidEntityHandle;
    }
    auto const parent = mNodes[GetSceneGraphIndex(tChild)].parent;
    return parent != kInvalidIndex ? mNodes[parent].entity : kInvalidEntityHandle;
}

auto SceneGraph::GetChild(SceneGraphHandle const tParent) const -> EntityHandle
{
    if (!isValid(tParent))
    {
        return kInvalidEntityHandle;
    }
    auto const child = mNodes[GetSceneGraphIndex(tParent)].leftChild;
    return child != kInvalidIndex ? mNodes[child].entity : kInvalidEntityHandle;
}

auto SceneGraph::GetNextSibling(SceneGraphHandle const tChild) const -> EntityHandle
{
    if (!isValid(tChild))
    {
        return kInvalidEntityHandle;
    }

    auto const sibling = mNodes[GetSceneGraphIndex(tChild)].rightSibling;
    return sibling != kInvalidIndex ? mNodes[sibling].entity : kInvalidEntityHandle;
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

    return index < mNodes.size() && mNodes[index].entity.index != kInvalidEntityIndex &&
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
        node.entity     = kInvalidEntityHandle;
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

}// namespace polos::ecs
