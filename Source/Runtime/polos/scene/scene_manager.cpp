///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/scene/scene_manager.hpp"

#include "polos/ecs/component.hpp"
#include "polos/ecs/component_types.hpp"
#include "polos/ecs/components/info_component.hpp"
#include "polos/ecs/components/light_component.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/scene/scene.hpp"
#include "polos/scene/scene_error_domain.hpp"
#include "polos/utils/assert.hpp"
#include "polos/utils/string_map.hpp"
#include "polos/utils/yaml_read.hpp"

#include <ryml/ryml_std.hpp>

#include <algorithm>
#include <array>
#include <fstream>
#include <span>
#include <string>
#include <system_error>

namespace polos::scene
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

void WriteId(ryml::NodeRef tNode, utils::string_id tId)
{
    auto const name = utils::GetStringFromID(tId);
    if (!name.empty())
    {
        tNode << ryml::csubstr{name.data(), name.size()};
        tNode |= ryml::VAL_DQUO;
        return;
    }
    tNode << tId;
    tNode.set_val_tag("!id");
}

auto ReadId(ryml::ConstNodeRef tNode) -> utils::string_id
{
    if (tNode.readable() && tNode.has_val_tag() && tNode.val_tag() == "!id")
    {
        return utils::yaml::Read<utils::string_id>(tNode);
    }
    return utils::StrHash64Runtime(utils::yaml::Scalar(tNode));
}

void WriteVec3(ryml::NodeRef tNode, glm::vec3 tValue)
{
    tNode |= ryml::SEQ;
    tNode |= ryml::FLOW_SL;
    tNode.append_child() << tValue.x;
    tNode.append_child() << tValue.y;
    tNode.append_child() << tValue.z;
}

template<typename T>
auto ReadField(ryml::ConstNodeRef tNode, char const* tKey, T tDefault) -> T
{
    return utils::yaml::Read(utils::yaml::Find(tNode, ryml::to_csubstr(tKey)), tDefault);
}

auto ReadVec3(ryml::ConstNodeRef tNode, char const* tKey, glm::vec3 tDefault) -> glm::vec3
{
    auto const value = utils::yaml::ReadArray(utils::yaml::Find(tNode, ryml::to_csubstr(tKey)), std::array{tDefault.x, tDefault.y, tDefault.z});
    return {value[0], value[1], value[2]};
}

auto WriteComponents(Scene& tScene, ecs::EntityHandle tHandle, ryml::NodeRef tEntity) -> void
{
    if (tScene.HasComponent<ecs::TransformComponent>(tHandle))
    {
        auto const& value = *tScene.GetComponent<ecs::TransformComponent>(tHandle);
        auto        node  = tEntity["transform"];
        node |= ryml::MAP;
        WriteVec3(node["position"], value.position);
        WriteVec3(node["rotation"], value.rotation);
        node["scale"] << value.scale;
    }
    if (tScene.HasComponent<ecs::MeshRendererComponent>(tHandle))
    {
        auto const& value = *tScene.GetComponent<ecs::MeshRendererComponent>(tHandle);
        auto        node  = tEntity["mesh_renderer"];
        node |= ryml::MAP;
        WriteId(node["mesh"], value.meshId);
        WriteId(node["material"], value.materialId);
    }
    if (tScene.HasComponent<ecs::VisibilityComponent>(tHandle))
    {
        auto node = tEntity["visibility"];
        node |= ryml::MAP;
        node["isVisible"].set_val(tScene.GetComponent<ecs::VisibilityComponent>(tHandle)->isVisible ? "true" : "false");
    }
    if (tScene.HasComponent<ecs::CameraComponent>(tHandle))
    {
        auto const& value = *tScene.GetComponent<ecs::CameraComponent>(tHandle);
        auto        node  = tEntity["camera"];
        node |= ryml::MAP;
        node["cameraType"].set_val(value.cameraType == ecs::CameraType::kOrtho ? ryml::csubstr{"orthographic"} : ryml::csubstr{"perspective"});
        node["nearZ"] << value.nearZ;
        node["farZ"] << value.farZ;
        node["fov"] << value.fov;
        node["size"] << value.size;
    }
    if (tScene.HasComponent<ecs::EditorCameraControllerComponent>(tHandle))
    {
        auto const& value = *tScene.GetComponent<ecs::EditorCameraControllerComponent>(tHandle);
        auto        node  = tEntity["editor_camera_controller"];
        node |= ryml::MAP;
        node["moveSpeed"] << value.moveSpeed;
        node["lookSensitivity"] << value.lookSensitivity;
    }
    if (tScene.HasComponent<ecs::LightComponent>(tHandle))
    {
        auto const& value = *tScene.GetComponent<ecs::LightComponent>(tHandle);
        auto        node  = tEntity["light"];
        node |= ryml::MAP;
        WriteVec3(node["color"], value.color);
        node["ambientFactor"] << value.ambientFactor;
        node["specularFactor"] << value.specularFactor;
    }
}

template<typename Reader>
auto ReadComponent(Scene& tScene, ecs::EntityHandle tHandle, ryml::ConstNodeRef tNode, Reader tRead) -> bool
{
    if (!tNode.readable())
    {
        return true;
    }
    if (!tNode.is_map())
    {
        return false;
    }
    return tScene.AddComponent(tHandle, tRead(tNode)) != nullptr;
}

auto ReadComponents(Scene& tScene, ecs::EntityHandle tHandle, ryml::ConstNodeRef tEntity) -> bool
{
    auto const read = [&](char const* tKey, auto tRead) {
        return ReadComponent(tScene, tHandle, utils::yaml::Find(tEntity, ryml::to_csubstr(tKey)), tRead);
    };

    std::string_view cameraType = "perspective";
    if (auto const camera = utils::yaml::Find(tEntity, "camera"); camera.readable() && camera.is_map())
    {
        cameraType = ReadField(camera, "cameraType", cameraType);
        if (cameraType != "perspective" && cameraType != "orthographic")
        {
            return false;
        }
    }

    return read("transform", [](ryml::ConstNodeRef node) {
        ecs::TransformComponent value{};
        value.position   = ReadVec3(node, "position", value.position);
        value.rotation   = ReadVec3(node, "rotation", value.rotation);
        auto const scale = utils::yaml::Find(node, "scale");
        if (scale.readable() && scale.is_seq())
        {
            value.scale = utils::yaml::Read(scale[0], value.scale);
        }
        else
        {
            value.scale = utils::yaml::Read(scale, value.scale);
        }
        return value;
    }) && read("mesh_renderer", [](ryml::ConstNodeRef node) {
        ecs::MeshRendererComponent value{.meshId = ReadId(utils::yaml::Find(node, "mesh"))};
        if (auto const material = utils::yaml::Find(node, "material"); material.readable())
        {
            value.materialId = ReadId(material);
        }
        return value;
    }) && read("visibility", [](ryml::ConstNodeRef node) {
        ecs::VisibilityComponent value{};
        value.isVisible = ReadField(node, "isVisible", value.isVisible);
        return value;
    }) && read("camera", [&](ryml::ConstNodeRef node) {
        ecs::CameraComponent value{};
        value.cameraType = cameraType == "orthographic" ? ecs::CameraType::kOrtho : ecs::CameraType::kPerspective;
        value.nearZ      = ReadField(node, "nearZ", value.nearZ);
        value.farZ       = ReadField(node, "farZ", value.farZ);
        value.fov        = ReadField(node, "fov", value.fov);
        value.size       = ReadField(node, "size", value.size);
        return value;
    }) && read("editor_camera_controller", [](ryml::ConstNodeRef node) {
        ecs::EditorCameraControllerComponent value{};
        value.moveSpeed       = ReadField(node, "moveSpeed", value.moveSpeed);
        value.lookSensitivity = ReadField(node, "lookSensitivity", value.lookSensitivity);
        return value;
    }) && read("light", [](ryml::ConstNodeRef node) {
        ecs::LightComponent value{};
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

SceneManager::SceneManager()
{
    sInstance = this;
}

SceneManager::~SceneManager()
{
    sInstance = nullptr;
}

auto SceneManager::CreateScene(std::string_view const tName, bool tEditorCamera) -> Scene*
{
    auto& scene = mScenes.emplace_back(std::make_unique<Scene>(utils::StrHash64Runtime(tName), tEditorCamera));
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
    ryml::Tree            document;
};

auto Scene::TakeSnapshot() -> Result<std::shared_ptr<SceneSnapshot const>>
{
    auto const destination = utils::GetStringFromID(name);
    if (destination.empty())
    {
        return ErrorType{SceneErrc::kMissingScenePath};
    }

    std::vector<ecs::EntityIndex>  indices(mEntitiesHead, ecs::kInvalidEntityIndex);
    std::vector<ecs::EntityHandle> handles;
    handles.reserve(mTotalEntities);
    for (ecs::EntityIndex i{0U}; i < mEntitiesHead; ++i)
    {
        auto const handle = mEntities[i].handle;
        if (isEntityValid(handle))
        {
            indices[i] = static_cast<ecs::EntityIndex>(handles.size());
            handles.push_back(handle);
        }
    }

    ryml::Tree document;
    auto       root = document.rootref();
    root |= ryml::MAP;
    auto entities = root["scene"];
    entities |= ryml::SEQ;
    for (auto const handle : handles)
    {
        auto entity = entities.append_child();
        entity |= ryml::MAP;
        auto info = entity["info"];
        info |= ryml::MAP;
        WriteId(info["debugName"], getComponent<ecs::InfoComponent>(handle.index)->debugName);

        auto children = info["children"];
        children |= ryml::SEQ;
        children |= ryml::FLOW_SL;
        for (auto child = GetChild(handle); ecs::IsEntityValid(child); child = GetNextSibling(child))
        {
            children.append_child() << indices[child.index];
        }
        WriteComponents(*this, handle, entity);
    }

    if (isEntityValid(activeCamera) && HasComponent<ecs::CameraComponent>(activeCamera))
    {
        root["activeCamera"] << indices[activeCamera.index];
    }
    else
    {
        root["activeCamera"].set_val("null");
    }

    return std::make_shared<SceneSnapshot const>(std::filesystem::path(destination), std::move(document));
}

auto SceneManager::TakeCurrentSceneSnapshot() -> Result<std::shared_ptr<SceneSnapshot const>>
{
    if (mCurrentScene == nullptr)
    {
        return ErrorType{SceneErrc::kNoCurrentScene};
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
    auto const output = ryml::emitrs_yaml<std::string>(tSnapshot.document);
    if (output.empty())
    {
        return ErrorType{SceneErrc::kSceneSerializationFailed};
    }

    auto temporary = tSnapshot.destination;
    temporary += ".tmp";

    std::ofstream file(temporary, std::ios::noreplace);
    if (!file)
    {
        return ErrorType{SceneErrc::kSceneFileOpenFailed};
    }

    file << output;
    file.close();

    std::error_code error;
    if (!file)
    {
        std::filesystem::remove(temporary, error);
        return ErrorType{SceneErrc::kSceneFileWriteFailed};
    }

    std::filesystem::rename(temporary, tSnapshot.destination, error);
    if (error)
    {
        std::filesystem::remove(temporary, error);
        return ErrorType{SceneErrc::kSceneFileReplaceFailed};
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

    auto const tree     = utils::yaml::Parse(file);
    auto const document = tree.crootref();
    if (!document.readable() || !document.is_map())
    {
        LogError("Invalid document in scene: {}", tScene);
        return nullptr;
    }

    auto const entitySequence = utils::yaml::Find(document, "scene");
    if (!entitySequence.readable() || !entitySequence.is_seq() || !entitySequence.has_children() || entitySequence.num_children() > ecs::Settings::kMaxEntities)
    {
        LogError("Invalid entity sequence in scene: {}", tScene);
        return nullptr;
    }

    std::vector<ryml::ConstNodeRef> entities;
    entities.reserve(entitySequence.num_children());
    for (auto entity : entitySequence.children())
    {
        entities.push_back(entity);
    }

    auto scene = std::unique_ptr<Scene>(new Scene(utils::StrHash64Runtime(tScene), false));
    // Allocate in file order, even when an entity refers to a parent at a higher index.
    for (ecs::EntityIndex i{0U}; i < entities.size(); ++i)
    {
        auto const entity = entities[i];
        auto const info   = utils::yaml::Find(entity, "info");
        auto const name   = ReadId(utils::yaml::Find(info, "debugName"));
        if (i == 0U)
        {
            scene->getComponent<ecs::InfoComponent>(scene->sceneRoot.index)->debugName = name;
            continue;
        }

        auto const handle = scene->newEntity();
        scene->addComponent(handle.index, ecs::InfoComponent{.debugName = name, .graphHandle = kInvalidSceneGraphHandle});
    }

    // Visit from the root so each parent already has a graph handle when its children are attached.
    std::vector<ecs::EntityIndex> parents{0U};
    parents.reserve(entities.size());
    for (std::size_t i{0U}; i < parents.size(); ++i)
    {
        auto const parent   = parents[i];
        auto*      info     = scene->getComponent<ecs::InfoComponent>(parent);
        auto const children = utils::yaml::Find(utils::yaml::Find(entities[parent], "info"), "children");
        if (!children.readable())
        {
            continue;
        }
        if (!children.is_seq() || children.num_children() >= entities.size())
        {
            LogError("Invalid children for entity {} in scene {}", parent, tScene);
            return nullptr;
        }

        // AddChild prepends. Attach in reverse to reproduce the listed sibling order.
        for (auto node = children.last_child(); node.readable(); node = node.prev_sibling())
        {
            auto const child = utils::yaml::Read(node, ecs::kInvalidEntityIndex);
            if (child == 0U || child >= entities.size())
            {
                LogError("Invalid child index {} in scene {}", child, tScene);
                return nullptr;
            }

            auto* childInfo = scene->getComponent<ecs::InfoComponent>(child);
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

    for (ecs::EntityIndex i{0U}; i < entities.size(); ++i)
    {
        if (!ReadComponents(*scene, scene->GetEntityAtIndex(i), entities[i]))
        {
            LogError("Invalid components for entity {} in scene {}", i, tScene);
            return nullptr;
        }
    }

    if (auto const activeCamera = utils::yaml::Find(document, "activeCamera");
        activeCamera.readable() && !(activeCamera.has_val() && activeCamera.val_is_null()))
    {
        auto const index = utils::yaml::Read(activeCamera, ecs::kInvalidEntityIndex);
        if (index >= entities.size() || !scene->SetActiveCamera(scene->GetEntityAtIndex(index)))
        {
            LogError("Invalid active camera in scene {}", tScene);
            return nullptr;
        }
    }

    mCurrentScene = mScenes.emplace_back(std::move(scene)).get();
    return mCurrentScene;
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
//                 scene->GetEntityAtIndex(index), MeshRendererComponent{.meshId = rendering::kDefaultQuad, .materialId = }) ==
//                 nullptr)
//         {
//             return nullptr;
//         }
//     }

//     mScenes.push_back(std::move(owned));
//     mCurrentScene = scene;
//     return scene;
// }

}// namespace polos::scene
