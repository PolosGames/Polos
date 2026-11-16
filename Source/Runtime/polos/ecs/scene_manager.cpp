///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/scene_manager.hpp"

#include "polos/ecs/component.hpp"
#include "polos/ecs/component_types.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/components/light_component.hpp"
#include "polos/ecs/ecs_error_domain.hpp"
#include "polos/ecs/scene.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <span>
#include <string>
#include <system_error>

namespace polos::ecs
{

namespace
{
// struct ComponentHeader
// {
//     std::uint8_t  type{0U};
//     std::uint64_t end{0U};
// };

// struct SceneHeader
// {
//     std::uint64_t                               entitiesEnd{0U};
//     std::array<ComponentHeader, kMaxComponents> componentHeaders;
// };

// constexpr std::uint32_t kSceneStart{sizeof(SceneHeader)};
// // Name + Active Camera's Entity(EntityIndex)
// constexpr std::uint32_t kSceneEnd = kSceneStart + sizeof(utils::string_id) + sizeof(EntityIndex);

// template<ComponentType Type>
// auto LoadComponentSection(
//     std::ifstream&                    tFile,
//     Scene&                            tScene,
//     SceneHeader const&                tHeader,
//     std::span<SerializedEntity const> tEntities) -> bool
// {
//     using ComponentT             = typename ComponentEnumToType<Type>::Type;
//     constexpr auto typeIndex     = static_cast<std::size_t>(Type);
//     constexpr auto componentMask = std::uint32_t{1} << typeIndex;

//     auto const sectionStart = tHeader.componentHeaders[typeIndex - 1U].end;
//     tFile.seekg(static_cast<std::streamoff>(sectionStart));
//     if (!tFile)
//     {
//         return false;
//     }

//     for (EntityIndex entityIndex{1U}; entityIndex < tEntities.size(); ++entityIndex)
//     {
//         if ((tEntities[entityIndex].componentTypes & componentMask) == 0U)
//         {
//             continue;
//         }

//         ComponentT component{};
//         if (!tFile.read(reinterpret_cast<char*>(&component), sizeof(component)))
//         {
//             return false;
//         }

//         auto const entity = tScene.GetEntityAtIndex(entityIndex);
//         if (tScene.AddComponent(entity, component) == nullptr)
//         {
//             return false;
//         }
//     }
//     return true;
// }

// constexpr std::array kComponentLoaders{
//     &LoadComponentSection<ComponentType::kVisibility>,
//     &LoadComponentSection<ComponentType::kCamera>,
//     &LoadComponentSection<ComponentType::kTransform>,
//     &LoadComponentSection<ComponentType::kEditorCameraController>,
//     &LoadComponentSection<ComponentType::kMeshRenderer>,
// };

auto WriteId(utils::string_id tId) -> YAML::Node
{
    auto const name = utils::GetStringFromID(tId);
    if (!name.empty())
    {
        return YAML::Node(std::string(name));
    }

    YAML::Node node(tId);
    node.SetTag("!id");
    return node;
}

auto ReadId(YAML::Node const& tNode) -> utils::string_id
{
    if (tNode.Tag() == "!id")
    {
        return tNode.as<utils::string_id>();
    }
    return utils::StrHash64Runtime(tNode.as<std::string>());
}

auto WriteVec3(glm::vec3 tValue) -> YAML::Node
{
    YAML::Node node(std::array{tValue.x, tValue.y, tValue.z});
    node.SetStyle(YAML::EmitterStyle::Flow);
    return node;
}

template<typename T>
auto ReadField(YAML::Node const& tNode, char const* tKey, T tDefault) -> T
{
    auto const field = tNode[tKey];
    return field ? field.as<T>() : tDefault;
}

auto ReadVec3(YAML::Node const& tNode, char const* tKey, glm::vec3 tDefault) -> glm::vec3
{
    auto const value = ReadField(tNode, tKey, std::array{tDefault.x, tDefault.y, tDefault.z});
    return {value[0], value[1], value[2]};
}

auto WriteComponents(Scene& tScene, EntityHandle tHandle, YAML::Node& tEntity) -> void
{
    if (tScene.HasComponent<TransformComponent>(tHandle))
    {
        auto const& value = *tScene.GetComponent<TransformComponent>(tHandle);
        auto        node  = tEntity["transform"];
        node["position"]  = WriteVec3(value.position);
        node["rotation"]  = WriteVec3(value.rotation);
        node["scale"]     = value.scale;
    }
    if (tScene.HasComponent<MeshRendererComponent>(tHandle))
    {
        auto const& value = *tScene.GetComponent<MeshRendererComponent>(tHandle);
        auto        node  = tEntity["mesh_renderer"];
        node["mesh"]      = WriteId(value.meshHandle);
        node["material"]  = WriteId(value.matHandle);
    }
    if (tScene.HasComponent<VisibilityComponent>(tHandle))
    {
        tEntity["visibility"]["isVisible"] = tScene.GetComponent<VisibilityComponent>(tHandle)->isVisible;
    }
    if (tScene.HasComponent<CameraComponent>(tHandle))
    {
        auto const& value  = *tScene.GetComponent<CameraComponent>(tHandle);
        auto        node   = tEntity["camera"];
        node["cameraType"] = value.cameraType == CameraType::kOrtho ? "orthographic" : "perspective";
        node["nearZ"]      = value.nearZ;
        node["farZ"]       = value.farZ;
        node["fov"]        = value.fov;
        node["size"]       = value.size;
    }
    if (tScene.HasComponent<EditorCameraControllerComponent>(tHandle))
    {
        auto const& value       = *tScene.GetComponent<EditorCameraControllerComponent>(tHandle);
        auto        node        = tEntity["editor_camera_controller"];
        node["moveSpeed"]       = value.moveSpeed;
        node["lookSensitivity"] = value.lookSensitivity;
    }
    if (tScene.HasComponent<LightComponent>(tHandle))
    {
        auto const& value      = *tScene.GetComponent<LightComponent>(tHandle);
        auto        node       = tEntity["light"];
        node["color"]          = WriteVec3(value.color);
        node["ambientFactor"]  = value.ambientFactor;
        node["specularFactor"] = value.specularFactor;
    }
}

template<typename Reader>
auto ReadComponent(Scene& tScene, EntityHandle tHandle, YAML::Node const& tNode, Reader tRead) -> bool
{
    if (!tNode)
    {
        return true;
    }
    if (!tNode.IsMap())
    {
        return false;
    }
    return tScene.AddComponent(tHandle, tRead(tNode)) != nullptr;
}

auto ReadComponents(Scene& tScene, EntityHandle tHandle, YAML::Node const& tEntity) -> bool
{
    auto const read = [&](char const* tKey, auto tRead) {
        return ReadComponent(tScene, tHandle, tEntity[tKey], tRead);
    };

    return read("transform", [](YAML::Node const& node) {
        TransformComponent value{};
        value.position = ReadVec3(node, "position", value.position);
        value.rotation = ReadVec3(node, "rotation", value.rotation);
        value.scale    = ReadField(node, "scale", value.scale);
        return value;
    }) && read("mesh_renderer", [](YAML::Node const& node) {
        MeshRendererComponent value{.meshHandle = ReadId(node["mesh"])};
        if (auto const material = node["material"]; material)
        {
            value.matHandle = ReadId(material);
        }
        return value;
    }) && read("visibility", [](YAML::Node const& node) {
        VisibilityComponent value{};
        value.isVisible = ReadField(node, "isVisible", value.isVisible);
        return value;
    }) && read("camera", [](YAML::Node const& node) {
        CameraComponent value{};
        auto const      type = ReadField(node, "cameraType", std::string("perspective"));
        if (type == "orthographic")
        {
            value.cameraType = CameraType::kOrtho;
        }
        else if (type != "perspective")
        {
            throw YAML::RepresentationException(node.Mark(), "Unknown camera type");
        }
        value.nearZ = ReadField(node, "nearZ", value.nearZ);
        value.farZ  = ReadField(node, "farZ", value.farZ);
        value.fov   = ReadField(node, "fov", value.fov);
        value.size  = ReadField(node, "size", value.size);
        return value;
    }) && read("editor_camera_controller", [](YAML::Node const& node) {
        EditorCameraControllerComponent value{};
        value.moveSpeed       = ReadField(node, "moveSpeed", value.moveSpeed);
        value.lookSensitivity = ReadField(node, "lookSensitivity", value.lookSensitivity);
        return value;
    }) && read("light", [](YAML::Node const& node) {
        LightComponent value{};
        value.color          = ReadVec3(node, "color", value.color);
        value.ambientFactor  = ReadField(node, "ambientFactor", value.ambientFactor);
        value.specularFactor = ReadField(node, "specularFactor", value.specularFactor);
        return value;
    });
}

}// namespace

SceneManager* SceneManager::sInstance = nullptr;

auto SceneManager::Get() -> SceneManager&
{
    return *sInstance;
}

SceneManager::SceneManager()  = default;
SceneManager::~SceneManager() = default;

auto SceneManager::CreateScene(utils::string_id const tName) -> Scene*
{
    auto& scene = mScenes.emplace_back(std::make_unique<Scene>(tName));
    MakeCurrentScene(scene->name);
    return scene.get();
}

auto SceneManager::MakeCurrentScene(utils::string_id const tName) -> bool
{
    if (auto result = QueryScene(tName); result.second)
    {
        mCurrentScene = result.first;
        return true;
    }

    return false;
}

auto SceneManager::QueryScene(utils::string_id const tName) -> utils::Pair<Scene*, bool>
{
    auto itr = std::ranges::find(mScenes, tName, &Scene::name);
    if (itr == mScenes.end())
    {
        LogError("Failed to find requested scene in scene library.");
        return {.first = nullptr, .second = false};
    }

    return {.first = itr->get(), .second = true};
}

auto SceneManager::GetCurrentScene() -> Scene*
{
    return mCurrentScene;
}

struct SceneSnapshot
{
    std::filesystem::path destination;
    YAML::Node document;
};

auto Scene::TakeSnapshot() -> Result<std::shared_ptr<SceneSnapshot const>>
{
    auto const destination = utils::GetStringFromID(name);
    if (destination.empty())
    {
        return ErrorType{EcsErrc::kMissingScenePath};
    }

    try
    {
        std::vector<EntityIndex>  indices(mEntitiesHead, kInvalidEntityIndex);
        std::vector<EntityHandle> handles;
        handles.reserve(mTotalEntities);
        for (EntityIndex i{0U}; i < mEntitiesHead; ++i)
        {
            auto const handle = mEntities[i].handle;
            if (isEntityValid(handle))
            {
                indices[i] = static_cast<EntityIndex>(handles.size());
                handles.push_back(handle);
            }
        }

        YAML::Node entities(YAML::NodeType::Sequence);
        for (auto const handle : handles)
        {
            YAML::Node entity(YAML::NodeType::Map);
            auto       info   = entity["info"];
            info["debugName"] = WriteId(getComponent<InfoComponent>(handle.index)->debugName);

            YAML::Node children(YAML::NodeType::Sequence);
            children.SetStyle(YAML::EmitterStyle::Flow);
            for (auto child = GetChild(handle); IsEntityValid(child); child = GetNextSibling(child))
            {
                children.push_back(indices[child.index]);
            }
            info["children"] = children;
            WriteComponents(*this, handle, entity);
            entities.push_back(entity);
        }

        YAML::Node document;
        document["scene"] = entities;
        if (isEntityValid(activeCamera) && HasComponent<CameraComponent>(activeCamera))
        {
            document["activeCamera"] = indices[activeCamera.index];
        }
        else
        {
            document["activeCamera"] = YAML::Null;
        }

        return std::make_shared<SceneSnapshot const>(std::filesystem::path(destination), std::move(document));
    }
    catch (YAML::Exception const&)
    {
        return ErrorType{EcsErrc::kSceneSerializationFailed};
    }
}

auto SceneManager::TakeCurrentSceneSnapshot() -> Result<std::shared_ptr<SceneSnapshot const>>
{
    if (mCurrentScene == nullptr)
    {
        return ErrorType{EcsErrc::kNoCurrentScene};
    }
    return mCurrentScene->TakeSnapshot();
}

auto SceneManager::SaveCurrentScene() -> Result<bool>
{
    auto snapshot = TakeCurrentSceneSnapshot();
    if (!snapshot)
    {
        return ErrorType{snapshot.error()};
    }
    return SaveSceneSnapshot(**snapshot);
}

auto SceneManager::SaveSceneSnapshot(SceneSnapshot const& tSnapshot) -> Result<bool>
{
    YAML::Emitter output;
    try
    {
        output << tSnapshot.document;
    }
    catch (YAML::Exception const&)
    {
        return ErrorType{EcsErrc::kSceneSerializationFailed};
    }
    if (!output.good())
    {
        return ErrorType{EcsErrc::kSceneSerializationFailed};
    }

    auto temporary = tSnapshot.destination;
    temporary += ".tmp";

    std::ofstream file(temporary, std::ios::noreplace);
    if (!file)
    {
        return ErrorType{EcsErrc::kSceneFileOpenFailed};
    }

    file << output.c_str() << '\n';
    file.close();

    std::error_code error;
    if (!file)
    {
        std::filesystem::remove(temporary, error);
        return ErrorType{EcsErrc::kSceneFileWriteFailed};
    }

    std::filesystem::rename(temporary, tSnapshot.destination, error);
    if (error)
    {
        std::filesystem::remove(temporary, error);
        return ErrorType{EcsErrc::kSceneFileReplaceFailed};
    }
    return true;
}

auto SceneManager::LoadScene(std::string_view const tScene) -> Scene*
{
    std::ifstream file{std::filesystem::path(tScene)};
    if (!file)
    {
        LogError("Could not open scene: {}", tScene);
        return nullptr;
    }

    try
    {
        auto const document = YAML::Load(file);
        auto const entities = document["scene"];
        if (!entities.IsSequence() || entities.size() == 0 || entities.size() > Settings::kMaxEntities)
        {
            LogError("Invalid entity sequence in scene: {}", tScene);
            return nullptr;
        }

        auto scene = std::unique_ptr<Scene>(new Scene(utils::StrHash64Runtime(tScene), false));
        // Allocate in file order, even when an entity refers to a parent at a higher index.
        for (EntityIndex i{0U}; i < entities.size(); ++i)
        {
            auto const entity = entities[i];
            auto const info   = entity["info"];
            auto const name   = ReadId(info["debugName"]);
            if (i == 0U)
            {
                scene->getComponent<InfoComponent>(scene->sceneRoot.index)->debugName = name;
                continue;
            }

            auto const handle = scene->newEntity();
            scene->addComponent(
                handle.index, InfoComponent{.debugName = name, .graphHandle = kInvalidSceneGraphHandle});
        }

        // Visit from the root so each parent already has a graph handle when its children are attached.
        std::vector<EntityIndex> parents{0U};
        parents.reserve(entities.size());
        for (std::size_t i{0U}; i < parents.size(); ++i)
        {
            auto const parent   = parents[i];
            auto*      info     = scene->getComponent<InfoComponent>(parent);
            auto const children = entities[parent]["info"]["children"];
            if (!children)
            {
                continue;
            }
            if (!children.IsSequence() || children.size() >= entities.size())
            {
                LogError("Invalid children for entity {} in scene {}", parent, tScene);
                return nullptr;
            }

            // AddChild prepends. Attach in reverse to reproduce the listed sibling order.
            for (auto childIndex = children.size(); childIndex > 0U; --childIndex)
            {
                auto const child = children[childIndex - 1U].as<EntityIndex>();
                if (child == 0U || child >= entities.size())
                {
                    LogError("Invalid child index {} in scene {}", child, tScene);
                    return nullptr;
                }

                auto* childInfo = scene->getComponent<InfoComponent>(child);
                if (childInfo->graphHandle != kInvalidSceneGraphHandle)
                {
                    LogError("Entity {} occurs more than once in the hierarchy in scene {}", child, tScene);
                    return nullptr;
                }

                auto const handle      = scene->GetEntityAtIndex(child);
                childInfo->graphHandle = scene->mSceneGraph.AddChild(info->graphHandle, handle);
                if (childInfo->graphHandle == kInvalidSceneGraphHandle)
                {
                    return nullptr;
                }

                ++info->childCount;
                parents.push_back(child);
                scene->mRenderData.meshInstanceChanges.push_back(handle);
            }
        }

        if (parents.size() != entities.size())
        {
            LogError("Scene {} contains entities outside the root hierarchy", tScene);
            return nullptr;
        }

        for (EntityIndex i{0U}; i < entities.size(); ++i)
        {
            if (!ReadComponents(*scene, scene->GetEntityAtIndex(i), entities[i]))
            {
                LogError("Invalid components for entity {} in scene {}", i, tScene);
                return nullptr;
            }
        }

        if (auto const activeCamera = document["activeCamera"]; activeCamera && !activeCamera.IsNull())
        {
            auto const index = activeCamera.as<EntityIndex>();
            if (index >= entities.size() || !scene->SetActiveCamera(scene->GetEntityAtIndex(index)))
            {
                LogError("Invalid active camera in scene {}", tScene);
                return nullptr;
            }
        }

        mCurrentScene = mScenes.emplace_back(std::move(scene)).get();
        return mCurrentScene;
    } catch (YAML::Exception const& error)
    {
        LogError("Could not read YAML scene {}: {}", tScene, error.what());
        return nullptr;
    }
}

// auto SceneManager::SaveScene(utils::string_id const tName) -> bool
// {
//     auto itr = std::ranges::find(mScenes, tName, &Scene::name);
//     if (itr == mScenes.end())
//     {
//         return false;
//     }

//     Scene* scene = itr->get();

//     // First order entities on a DFS basis
//     std::vector<EntityIndex> dfsEntities;
//     dfsEntities.reserve(scene->mTotalEntities);

//     {
//         EntityIndex currentIndex{0U};
//         auto&       sceneGraph = scene->mSceneGraph;

//         // Reorder entities with dfs
//         while (dfsEntities.size() < scene->mTotalEntities)
//         {
//             dfsEntities.push_back(currentIndex);

//             auto child = sceneGraph.GetChild(scene->getComponent<InfoComponent>(currentIndex)->graphHandle);
//             if (kInvalidEntityHandle != child)
//             {
//                 currentIndex = child.index;
//                 continue;
//             }

//             // traverse up to find another sibling
//             while (true)
//             {
//                 auto sibling = sceneGraph.GetNextSibling(scene->getComponent<InfoComponent>(currentIndex)->graphHandle);
//                 if (kInvalidEntityHandle != sibling)
//                 {
//                     currentIndex = sibling.index;
//                     break;
//                 }

//                 auto parent = sceneGraph.GetParent(scene->getComponent<InfoComponent>(currentIndex)->graphHandle);
//                 if (parent == kInvalidEntityHandle)// sceneRoot, also currentIndex == 0
//                 {
//                     break;
//                 }

//                 currentIndex = parent.index;
//             }

//             if (currentIndex == 0)
//             {
//                 break;
//             }
//         }
//     }

//     SceneHeader header{};
//     header.entitiesEnd = kSceneEnd + (sizeof(SerializedEntity) * dfsEntities.size());

//     std::uint64_t componentEnd = header.entitiesEnd;
//     for (std::uint8_t i{0U}; i < kMaxComponents; ++i)
//     {
//         header.componentHeaders[i].type = i;
//         header.componentHeaders[i].end =
//             componentEnd + (static_cast<std::uint64_t>(scene->mComponentPools[i].GetTotalComponents()) *
//                                static_cast<std::uint64_t>(scene->mComponentPools[i].elemSize));

//         componentEnd = header.componentHeaders[i].end;
//     }

//     std::vector<SerializedEntity> serializedEntities;
//     serializedEntities.reserve(dfsEntities.size());

//     std::array<std::uint32_t, kMaxComponents> componentCounts{};
//     for (std::size_t i{0U}; i < dfsEntities.size(); i++)
//     {
//         serializedEntities << scene->mEntities[dfsEntities[i]];

//         for (std::uint8_t type{0U}; type < kMaxComponents; ++type)
//         {
//             if ((serializedEntities.back().componentTypes & (std::uint32_t{1} << type)) != 0U)
//             {
//                 serializedEntities.back().componentIndices[type] = componentCounts[type]++;
//             }
//         }
//     }

//     EntityIndex activeCamera{kInvalidEntityIndex};
//     if (scene->isEntityValid(scene->activeCamera))
//     {
//         auto const camera = std::ranges::find(dfsEntities, scene->activeCamera.index);
//         if (camera != dfsEntities.end())
//         {
//             activeCamera = static_cast<EntityIndex>(camera - dfsEntities.begin());
//         }
//     }

//     auto const path = std::filesystem::path{utils::GetStringFromID(tName)};

//     std::ofstream file{path, std::ios::binary | std::ios::trunc};
//     if (!file.is_open())
//     {
//         return false;
//     }

//     file.write(reinterpret_cast<char const*>(&header), sizeof(SceneHeader));
//     file.write(reinterpret_cast<char const*>(&scene->name), sizeof(utils::string_id));
//     file.write(reinterpret_cast<char const*>(&activeCamera), sizeof(EntityIndex));
//     file.write(
//         reinterpret_cast<char const*>(serializedEntities.data()),
//         static_cast<std::streamsize>(serializedEntities.size() * sizeof(SerializedEntity)));

//     for (std::uint8_t cType{0U}; cType < kMaxComponents; ++cType)
//     {
//         auto const&   pool          = scene->mComponentPools[cType];
//         auto const    componentSize = pool.elemSize;
//         std::uint32_t componentIndex{0U};
//         for (std::size_t e{0U}; e < serializedEntities.size(); ++e)
//         {
//             if ((serializedEntities[e].componentTypes & (std::uint32_t{1} << cType)) == 0U)
//             {
//                 continue;
//             }

//             auto const* source = static_cast<std::uint8_t const*>(pool.mData) + (dfsEntities[e] * componentSize);
//             file.write(reinterpret_cast<char const*>(source), static_cast<std::streamsize>(componentSize));
//             ++componentIndex;
//         }
//         if (componentIndex != pool.GetTotalComponents() ||
//             file.tellp() != static_cast<std::streamoff>(header.componentHeaders[cType].end))
//         {
//             return false;
//         }
//     }

//     file.close();
//     return !file.fail();
// }

// auto SceneManager::LoadScene(std::string_view const tScene) -> Scene*
// {
//     std::unique_ptr<Scene> owned{new Scene{"Scene"_sid, false}};
//     auto*                  scene = owned.get();

//     SceneHeader header{};

//     std::filesystem::path const path = tScene;

//     std::ifstream file(path, std::ios::binary);
//     if (!file.is_open())
//     {
//         LogError("Could not open scene {}!", path.filename().string());
//         return nullptr;
//     }

//     EntityIndex activeCamera{kInvalidEntityIndex};
//     file.read(reinterpret_cast<char*>(&header), sizeof(SceneHeader));
//     file.read(reinterpret_cast<char*>(&scene->name), sizeof(utils::string_id));
//     file.read(reinterpret_cast<char*>(&activeCamera), sizeof(EntityIndex));
//     scene->activeCamera = EntityHandle{.index = activeCamera, .version = 0U};

//     auto const                    entitiesSize = header.entitiesEnd - kSceneEnd;
//     std::vector<SerializedEntity> serializedEntities(entitiesSize / sizeof(SerializedEntity));
//     file.read(reinterpret_cast<char*>(serializedEntities.data()), static_cast<std::streamsize>(entitiesSize));

//     auto const infoCompStart = header.entitiesEnd;
//     auto const infoCompEnd   = header.componentHeaders[0U].end;

//     auto const infoCompSize  = infoCompEnd - infoCompStart;
//     auto const infoCompCount = infoCompSize / sizeof(InfoComponent);

//     std::vector<InfoComponent> infoComponents(infoCompCount);
//     file.read(reinterpret_cast<char*>(infoComponents.data()), static_cast<std::streamsize>(infoCompSize));

//     {
//         auto& sceneGraph = scene->mSceneGraph;

//         EntityHandle               currentEntity{scene->sceneRoot};
//         std::uint32_t              serializedEntitiesIdx{1U};
//         std::vector<std::uint32_t> childCounts;
//         childCounts.push_back(infoComponents[serializedEntities[0U].componentIndices[0U]].childCount);

//         while (scene->mTotalEntities < serializedEntities.size())
//         {
//             currentEntity     = scene->NewEntity(EntityHandle{.index = currentEntity.index, .version = 0U});
//             auto* currentInfo = scene->getComponent<InfoComponent>(currentEntity.index);

//             currentInfo->debugName =
//                 infoComponents[serializedEntities[serializedEntitiesIdx].componentIndices[0U]].debugName;

//             childCounts.push_back(
//                 infoComponents[serializedEntities[serializedEntitiesIdx].componentIndices[0U]].childCount);

//             serializedEntitiesIdx++;
//             if (0 != childCounts.back())
//             {
//                 continue;
//             }

//             while (true)
//             {
//                 currentEntity = scene->GetParent(currentEntity);

//                 if (kInvalidEntityHandle == currentEntity)// no more children of sceneRoot
//                 {
//                     break;
//                 }

//                 childCounts.pop_back();
//                 auto remainingSibling = childCounts.back() - 1;
//                 if (0 != remainingSibling)
//                 {
//                     --childCounts.back();
//                     break;
//                 }
//             }
//         }

//         // NewEntity prepends we reverse each list to restore DFS sibling order.
//         for (auto& parent : sceneGraph.mNodes)
//         {
//             auto previous = SceneGraph::kInvalidIndex;
//             auto current  = parent.leftChild;
//             while (current != SceneGraph::kInvalidIndex)
//             {
//                 auto&      child      = sceneGraph.mNodes[current];
//                 auto const next       = child.rightSibling;
//                 child.rightSibling    = previous;
//                 child.previousSibling = next;
//                 previous              = current;
//                 current               = next;
//             }
//             parent.leftChild = previous;
//         }
//     }

//     for (auto const load : kComponentLoaders)
//     {
//         if (!load(file, *scene, header, serializedEntities))
//         {
//             return nullptr;
//         }
//     }

//     // Older scenes stored a separate quad payload in slot 6.
//     constexpr auto legacyQuadMask = std::uint32_t{1} << 6U;
//     constexpr auto meshMask       = std::uint32_t{1} << static_cast<std::uint8_t>(ComponentType::kMeshRenderer);
//     for (EntityIndex index{1U}; index < serializedEntities.size(); ++index)
//     {
//         if ((serializedEntities[index].componentTypes & (legacyQuadMask | meshMask)) == legacyQuadMask &&
//             scene->AddComponent(
//                 scene->GetEntityAtIndex(index), MeshRendererComponent{.meshHandle = rendering::kDefaultQuad, .matHandle = }) ==
//                 nullptr)
//         {
//             return nullptr;
//         }
//     }

//     mScenes.push_back(std::move(owned));
//     mCurrentScene = scene;
//     return scene;
// }

}// namespace polos::ecs
