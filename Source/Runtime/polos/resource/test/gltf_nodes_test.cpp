///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/resource/gltf/gltf_loader.hpp"
#include "polos/resource/model.hpp"

#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace polos::resource::model
{
class GltfNodesTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        directory = std::filesystem::temp_directory_path() /
                    ("polos-gltf-nodes-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directories(directory / "Models");
        std::ofstream descriptor(directory / "Models/model.yaml");
        descriptor << "id: model.test\npath: model.gltf\nmaterials: []\n";
        std::array<float, 9> const positions{0, 0, 0, 1, 0, 0, 0, 1, 0};
        std::ofstream              buffer(directory / "positions.bin", std::ios::binary);
        buffer.write(reinterpret_cast<char const*>(positions.data()), sizeof(positions));
    }

    void TearDown() override
    {
        std::filesystem::remove_all(directory);
    }

    auto Load(std::string_view nodesAndScenes) -> std::unique_ptr<Model>
    {
        std::ofstream file(directory / "model.gltf");
        file << R"({"asset":{"version":"2.0"},
"buffers":[{"uri":"positions.bin","byteLength":36}],
"bufferViews":[{"buffer":0,"byteLength":36}],
"accessors":[{"bufferView":0,"componentType":5126,"count":3,"type":"VEC3","min":[0,0,0],"max":[1,1,0]}],
"meshes":[{"primitives":[{"attributes":{"POSITION":0}}]}],)"
             << nodesAndScenes << "}";
        file.close();
        return LoadGltf(directory / "Models/model.yaml");
    }

    std::filesystem::path directory;
};

TEST_F(GltfNodesTest, PreservesHierarchyTransformsAndSharedMeshInstancesInDefaultScene)
{
    auto model = Load(R"(
"nodes":[
 {"name":"Parent","translation":[2,3,4],"rotation":[0,0,0,1],"scale":[2,2,2],"children":[1,2]},
 {"name":"First","mesh":0,"translation":[1,0,0],"rotation":[0,0,1,0]},
 {"name":"Second","mesh":0,"matrix":[3,0,0,0,0,3,0,0,0,0,3,0,5,6,7,1]},
 {"name":"OtherScene","mesh":0}
],
"scenes":[{"nodes":[3]},{"nodes":[0]}],"scene":1)");
    ASSERT_NE(model, nullptr);
    ASSERT_EQ(model->nodes.size(), 4U);
    EXPECT_EQ(model->rootNodes, (std::vector<std::size_t>{0}));
    EXPECT_EQ(model->nodes[0].children, (std::vector<std::size_t>{1, 2}));
    EXPECT_EQ(model->nodes[0].position, glm::vec3(2, 3, 4));
    EXPECT_FLOAT_EQ(model->nodes[0].scale, 2.0F);
    EXPECT_EQ(model->nodes[1].rotation, glm::quat(0, 0, 0, 1));
    EXPECT_EQ(model->nodes[1].meshIndex, model->nodes[2].meshIndex);
    EXPECT_EQ(model->nodes[2].position, glm::vec3(5, 6, 7));
    EXPECT_FLOAT_EQ(model->nodes[2].scale, 3.0F);
    ASSERT_EQ(model->meshes.size(), 1U);
    // Imported translations belong to nodes, leaving shared mesh vertices in local space.
    EXPECT_EQ(model->meshes[0].primitives[0].primitiveVertices[1].position, glm::vec3(1, 0, 0));
}

TEST_F(GltfNodesTest, FindsParentlessNodesWhenTheAssetHasNoScenes)
{
    auto model = Load(R"("nodes":[{"children":[1]},{"mesh":0},{"mesh":0}])");
    ASSERT_NE(model, nullptr);
    EXPECT_EQ(model->rootNodes, (std::vector<std::size_t>{0, 2}));
    EXPECT_EQ(model->nodes[1].position, glm::vec3(0));
    EXPECT_FLOAT_EQ(model->nodes[1].scale, 1.0F);
}
}// namespace polos::resource::model
