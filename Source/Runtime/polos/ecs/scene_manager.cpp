///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/scene_manager.hpp"

#include "polos/ecs/component.hpp"
#include "polos/ecs/component_types.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <span>
#include <string>

namespace polos::ecs
{

namespace
{
struct ComponentHeader
{
    std::uint8_t  type{0U};
    std::uint64_t end{0U};
};

struct SceneHeader
{
    std::uint64_t                               entitiesEnd{0U};
    std::array<ComponentHeader, kMaxComponents> componentHeaders;
};

constexpr std::uint32_t kSceneStart{sizeof(SceneHeader)};
// Name + Active Camera's Entity(EntityIndex)
constexpr std::uint32_t kSceneEnd = kSceneStart + sizeof(utils::string_id) + sizeof(EntityIndex);

template<ComponentType Type>
auto LoadComponentSection(
    std::ifstream&                    tFile,
    Scene&                            tScene,
    SceneHeader const&                tHeader,
    std::span<SerializedEntity const> tEntities) -> bool
{
    using ComponentT             = typename ComponentEnumToType<Type>::Type;
    constexpr auto typeIndex     = static_cast<std::size_t>(Type);
    constexpr auto componentMask = std::uint32_t{1} << typeIndex;

    auto const sectionStart = tHeader.componentHeaders[typeIndex - 1U].end;
    tFile.seekg(static_cast<std::streamoff>(sectionStart));
    if (!tFile)
    {
        return false;
    }

    for (EntityIndex entityIndex{1U}; entityIndex < tEntities.size(); ++entityIndex)
    {
        if ((tEntities[entityIndex].componentTypes & componentMask) == 0U)
        {
            continue;
        }

        ComponentT component{};
        if (!tFile.read(reinterpret_cast<char*>(&component), sizeof(component)))
        {
            return false;
        }

        auto const entity = tScene.GetEntityAtIndex(entityIndex);
        if (tScene.AddComponent(entity, component) == nullptr)
        {
            return false;
        }
    }
    return true;
}

constexpr std::array kComponentLoaders{
    &LoadComponentSection<ComponentType::kVisibility>,
    &LoadComponentSection<ComponentType::kCamera>,
    &LoadComponentSection<ComponentType::kTransform>,
    &LoadComponentSection<ComponentType::kEditorCameraController>,
    &LoadComponentSection<ComponentType::kQuadRenderer>,
};
}// namespace

SceneManager* SceneManager::sInstance     = nullptr;
Scene*        SceneManager::sCurrentScene = nullptr;

auto SceneManager::CreateScene() -> Scene*
{
    auto& scene = sInstance->mScenes.emplace_back(std::make_unique<Scene>());
    MakeCurrentScene(scene->name);
    return scene.get();
}

auto SceneManager::MakeCurrentScene(utils::string_id const tName) -> bool
{
    auto itr = std::ranges::find(sInstance->mScenes, tName, &Scene::name);
    if (itr == sInstance->mScenes.end())
    {
        LogError("Failed to find requested scene in scene library.");
        return false;
    }

    sCurrentScene = itr->get();

    return true;
}

auto SceneManager::GetCurrentScene() -> Scene*
{
    return sCurrentScene;
}

auto SceneManager::SaveScene(utils::string_id const tName) -> bool
{
    if (sInstance == nullptr)
    {
        return false;
    }
    auto itr = std::ranges::find(sInstance->mScenes, tName, &Scene::name);
    if (itr == sInstance->mScenes.end())
    {
        return false;
    }

    Scene* scene = itr->get();

    // First order entities on a DFS basis
    std::vector<EntityIndex> dfsEntities;
    dfsEntities.reserve(scene->mTotalEntities);

    {
        EntityIndex currentIndex{0U};
        auto&       sceneGraph = scene->mSceneGraph;

        // Reorder entities with dfs
        while (dfsEntities.size() < scene->mTotalEntities)
        {
            dfsEntities.push_back(currentIndex);

            auto child = sceneGraph.GetChild(scene->getComponent<InfoComponent>(currentIndex)->graphHandle);
            if (kInvalidEntityHandle != child)
            {
                currentIndex = child.index;
                continue;
            }

            // traverse up to find another sibling
            while (true)
            {
                auto sibling = sceneGraph.GetNextSibling(scene->getComponent<InfoComponent>(currentIndex)->graphHandle);
                if (kInvalidEntityHandle != sibling)
                {
                    currentIndex = sibling.index;
                    break;
                }

                auto parent = sceneGraph.GetParent(scene->getComponent<InfoComponent>(currentIndex)->graphHandle);
                if (parent == kInvalidEntityHandle)// sceneRoot, also currentIndex == 0
                {
                    break;
                }

                currentIndex = parent.index;
            }

            if (currentIndex == 0)
            {
                break;
            }
        }
    }

    SceneHeader header{};
    header.entitiesEnd = kSceneEnd + (sizeof(SerializedEntity) * dfsEntities.size());

    std::uint64_t componentEnd = header.entitiesEnd;
    for (std::uint8_t i{0U}; i < kMaxComponents; ++i)
    {
        header.componentHeaders[i].type = i;
        header.componentHeaders[i].end =
            componentEnd + (static_cast<std::uint64_t>(scene->mComponentPools[i].GetTotalComponents()) *
                            static_cast<std::uint64_t>(scene->mComponentPools[i].elemSize));

        componentEnd = header.componentHeaders[i].end;
    }

    std::vector<SerializedEntity> serializedEntities;
    serializedEntities.reserve(dfsEntities.size());

    std::array<std::uint32_t, kMaxComponents> componentCounts{};
    for (std::size_t i{0U}; i < dfsEntities.size(); i++)
    {
        serializedEntities << scene->mEntities[dfsEntities[i]];

        for (std::uint8_t type{0U}; type < kMaxComponents; ++type)
        {
            if ((serializedEntities.back().componentTypes & (std::uint32_t{1} << type)) != 0U)
            {
                serializedEntities.back().componentIndices[type] = componentCounts[type]++;
            }
        }
    }

    EntityIndex activeCamera{kInvalidEntityIndex};
    if (scene->isEntityValid(scene->activeCamera))
    {
        auto const camera = std::ranges::find(dfsEntities, scene->activeCamera.index);
        if (camera != dfsEntities.end())
        {
            activeCamera = static_cast<EntityIndex>(camera - dfsEntities.begin());
        }
    }

    auto const path = std::filesystem::path{utils::GetStringFromID(tName)};

    std::ofstream file{path, std::ios::binary | std::ios::trunc};
    if (!file.is_open())
    {
        return false;
    }

    file.write(reinterpret_cast<char const*>(&header), sizeof(SceneHeader));
    file.write(reinterpret_cast<char const*>(&scene->name), sizeof(utils::string_id));
    file.write(reinterpret_cast<char const*>(&activeCamera), sizeof(EntityIndex));
    file.write(
        reinterpret_cast<char const*>(serializedEntities.data()),
        static_cast<std::streamsize>(serializedEntities.size() * sizeof(SerializedEntity)));

    for (std::uint8_t cType{0U}; cType < kMaxComponents; ++cType)
    {
        auto const&   pool          = scene->mComponentPools[cType];
        auto const    componentSize = pool.elemSize;
        std::uint32_t componentIndex{0U};
        for (std::size_t e{0U}; e < serializedEntities.size(); ++e)
        {
            if ((serializedEntities[e].componentTypes & (std::uint32_t{1} << cType)) == 0U)
            {
                continue;
            }

            auto const* source = static_cast<std::uint8_t const*>(pool.mData) + (dfsEntities[e] * componentSize);
            file.write(reinterpret_cast<char const*>(source), static_cast<std::streamsize>(componentSize));
            ++componentIndex;
        }
        if (componentIndex != pool.GetTotalComponents() ||
            file.tellp() != static_cast<std::streamoff>(header.componentHeaders[cType].end))
        {
            return false;
        }
    }

    file.close();
    return !file.fail();
}

auto SceneManager::LoadScene(std::string_view const tScene) -> Scene*
{
    auto* scene = CreateScene();

    SceneHeader header{};

    std::filesystem::path const path = tScene;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        LogError("Could not open scene {}!", path.filename().string());
        return nullptr;
    }

    EntityIndex activeCamera{kInvalidEntityIndex};
    file.read(reinterpret_cast<char*>(&header), sizeof(SceneHeader));
    file.read(reinterpret_cast<char*>(&scene->name), sizeof(utils::string_id));
    file.read(reinterpret_cast<char*>(&activeCamera), sizeof(EntityIndex));
    scene->activeCamera = EntityHandle{.index = activeCamera, .version = 0U};

    auto const                    entitiesSize = header.entitiesEnd - kSceneEnd;
    std::vector<SerializedEntity> serializedEntities(entitiesSize / sizeof(SerializedEntity));
    file.read(reinterpret_cast<char*>(serializedEntities.data()), static_cast<std::streamsize>(entitiesSize));

    auto const infoCompStart = header.entitiesEnd;
    auto const infoCompEnd   = header.componentHeaders[0U].end;

    auto const infoCompSize  = infoCompEnd - infoCompStart;
    auto const infoCompCount = infoCompSize / sizeof(InfoComponent);

    std::vector<InfoComponent> infoComponents(infoCompCount);
    file.read(reinterpret_cast<char*>(infoComponents.data()), static_cast<std::streamsize>(infoCompSize));

    {
        auto& sceneGraph = scene->mSceneGraph;

        EntityHandle               currentEntity{scene->sceneRoot};
        std::uint32_t              serializedEntitiesIdx{1U};
        std::vector<std::uint32_t> childCounts;
        childCounts.push_back(infoComponents[serializedEntities[0U].componentIndices[0U]].childCount);

        while (scene->mTotalEntities < serializedEntities.size())
        {
            currentEntity     = scene->NewEntity(EntityHandle{.index = currentEntity.index, .version = 0U});
            auto* currentInfo = scene->getComponent<InfoComponent>(currentEntity.index);

            currentInfo->debugName =
                infoComponents[serializedEntities[serializedEntitiesIdx].componentIndices[0U]].debugName;

            childCounts.push_back(
                infoComponents[serializedEntities[serializedEntitiesIdx].componentIndices[0U]].childCount);

            serializedEntitiesIdx++;
            if (0 != childCounts.back())
            {
                continue;
            }

            while (true)
            {
                currentEntity = scene->GetParent(currentEntity);

                if (kInvalidEntityHandle == currentEntity)// no more children of sceneRoot
                {
                    break;
                }

                childCounts.pop_back();
                auto remainingSibling = childCounts.back() - 1;
                if (0 != remainingSibling)
                {
                    --childCounts.back();
                    break;
                }
            }
        }

        // NewEntity prepends we reverse each list to restore DFS sibling order.
        for (auto& parent : sceneGraph.mNodes)
        {
            auto previous = SceneGraph::kInvalidIndex;
            auto current  = parent.leftChild;
            while (current != SceneGraph::kInvalidIndex)
            {
                auto&      child      = sceneGraph.mNodes[current];
                auto const next       = child.rightSibling;
                child.rightSibling    = previous;
                child.previousSibling = next;
                previous              = current;
                current               = next;
            }
            parent.leftChild = previous;
        }
    }

    for (auto const load : kComponentLoaders)
    {
        if (!load(file, *scene, header, serializedEntities))
        {
            return nullptr;
        }
    }

    return scene;
}

}// namespace polos::ecs
