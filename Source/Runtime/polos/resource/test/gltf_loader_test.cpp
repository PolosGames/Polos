#include "polos/rendering/common.hpp"
#include "polos/resource/gltf/gltf_loader.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <array>
#include <atomic>
#include <bit>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <limits>

namespace
{
using Json         = nlohmann::json;
namespace resource = polos::resource::model;

class GltfLoaderTest : public ::testing::Test
{
protected:
    std::filesystem::path     directory;
    std::vector<std::uint8_t> bytes;
    Json                      document;

    void SetUp() override
    {
        static std::atomic<unsigned> serial{};
        directory = std::filesystem::temp_directory_path() /
                    ("polos-gltf-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" + std::to_string(serial++));
        std::filesystem::create_directories(directory / "Models");
        document = {
            {"asset", {{"version", "2.0"}}}, {"buffers", Json::array()}, {"bufferViews", Json::array()}, {"accessors", Json::array()},
            {"meshes", Json::array({{{"primitives", Json::array({{{"attributes", Json::object()}}})}}})}
        };
        auto view                              = AddView(Floats({0, 0, 0, 1, 0, 0, 0, 1, 0}));
        auto position                          = AddAccessor(view, "VEC3", 3, 5126);
        document["accessors"][position]["min"] = {0, 0, 0};
        document["accessors"][position]["max"] = {1, 1, 0};
        Primitive()["attributes"]["POSITION"]  = position;
    }

    void TearDown() override
    {
        std::filesystem::remove_all(directory);
    }

    auto Primitive() -> Json&
    {
        return document["meshes"][0]["primitives"][0];
    }

    static void AppendUnsigned(std::vector<std::uint8_t>& output, std::uint32_t value, unsigned width)
    {
        for (unsigned i = 0; i < width; ++i)
        {
            output.push_back(static_cast<std::uint8_t>(value >> (i * 8U)));
        }
    }

    static std::vector<std::uint8_t> Floats(std::initializer_list<float> values)
    {
        std::vector<std::uint8_t> result;
        for (float value : values)
        {
            AppendUnsigned(result, std::bit_cast<std::uint32_t>(value), 4);
        }
        return result;
    }

    auto AddView(std::vector<std::uint8_t> const& data, unsigned stride = 0) -> std::size_t
    {
        while (bytes.size() % 4 != 0)
        {
            bytes.push_back(0);
        }
        auto index = document["bufferViews"].size();
        Json view  = {{"buffer", 0}, {"byteOffset", bytes.size()}, {"byteLength", data.size()}};
        if (stride != 0)
        {
            view["byteStride"] = stride;
        }
        document["bufferViews"].push_back(view);
        bytes.insert(bytes.end(), data.begin(), data.end());
        return index;
    }

    auto AddAccessor(std::size_t view, std::string const& type, unsigned count, int component, bool normalized = false, unsigned offset = 0) -> std::size_t
    {
        auto index = document["accessors"].size();
        document["accessors"].push_back(
            {{"bufferView", view}, {"byteOffset", offset}, {"type", type}, {"componentType", component}, {"count", count}, {"normalized", normalized}});
        return index;
    }

    auto Write(std::string_view materialId = {}) -> std::filesystem::path
    {
        document["buffers"] = Json::array({{{"uri", "mesh.bin"}, {"byteLength", bytes.size()}}});
        std::ofstream binary(directory / "mesh.bin", std::ios::binary);
        binary.write(reinterpret_cast<char const*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        std::ofstream json(directory / "mesh.gltf");
        json << document.dump();
        auto const    descriptorPath = directory / "Models/mesh.yaml";
        std::ofstream descriptor(descriptorPath);
        descriptor << "id: model.test\npath: mesh.gltf\n";
        if (!materialId.empty())
        {
            descriptor << "materials:\n  - id: " << materialId << '\n';
        }
        return descriptorPath;
    }
};

TEST_F(GltfLoaderTest, LoadsPackedPositionsAndDefaultsWithoutIndices)
{
    auto result = resource::LoadGltf(Write());

    ASSERT_TRUE(result);

    auto const& model = result->model;

    ASSERT_EQ(model.meshes.size(), 1U);
    ASSERT_EQ(model.meshes[0].primitives.size(), 1U);
    ASSERT_EQ(result->primitives.size(), 1U);
    auto const& [id, part] = result->primitives[0];
    EXPECT_EQ(model.meshes[0].primitives[0], id);
    EXPECT_EQ(id, "model.test.mesh.0.0"_sid);
    EXPECT_EQ(part.primitiveIndices, (std::vector<std::uint32_t>{0, 1, 2}));
    EXPECT_EQ(part.material, polos::rendering::kDefaultMaterial);
    auto const& vertices = part.primitiveVertices;

    ASSERT_EQ(vertices.size(), 3U);
    EXPECT_FLOAT_EQ(vertices[1].position.x, 1);
    EXPECT_FLOAT_EQ(vertices[2].position.y, 1);
    EXPECT_EQ(vertices[0].color, glm::vec4(1));
    EXPECT_EQ(vertices[0].texCoord, glm::vec2(0));
    EXPECT_FLOAT_EQ(vertices[0].tangent.w, 1);
}

TEST_F(GltfLoaderTest, LoadsSeparatePositionsAndInterleavedNormalsAndUvsWithOffsets)
{
    auto view                               = AddView(Floats({0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1}), 20);
    Primitive()["attributes"]["NORMAL"]     = AddAccessor(view, "VEC3", 3, 5126);
    Primitive()["attributes"]["TEXCOORD_0"] = AddAccessor(view, "VEC2", 3, 5126, false, 12);
    auto tangentView                        = AddView(Floats({1, 0, 0, 1, 0, 1, 0, -1, 0, 0, 1, 1}));
    Primitive()["attributes"]["TANGENT"]    = AddAccessor(tangentView, "VEC4", 3, 5126);

    auto result = resource::LoadGltf(Write());

    ASSERT_TRUE(result);

    ASSERT_EQ(result->model.meshes.size(), 1U);
    ASSERT_EQ(result->model.meshes[0].primitives.size(), 1U);
    auto const& vertices = result->primitives[0].second.primitiveVertices;
    ASSERT_EQ(vertices.size(), 3U);

    EXPECT_EQ(vertices[0].normal, glm::vec3(0, 0, 1));
    EXPECT_EQ(vertices[1].normal, glm::vec3(0, 1, 0));
    EXPECT_EQ(vertices[2].normal, glm::vec3(1, 0, 0));
    EXPECT_EQ(vertices[1].texCoord, glm::vec2(1, 1));
    EXPECT_EQ(vertices[2].texCoord, glm::vec2(0, 0));
    EXPECT_EQ(vertices[1].tangent, glm::vec4(0, 1, 0, -1));
    EXPECT_FLOAT_EQ(vertices[2].position.y, 1);
}

TEST_F(GltfLoaderTest, DecodesNormalizedByteRgbBeforePositionAndKeepsAlphaOne)
{
    // RGB byte vertices require a four-byte stride, including padding.
    auto view                            = AddView({255, 128, 0, 0, 0, 255, 64, 0, 128, 0, 255, 0}, 4);
    Primitive()["attributes"]["COLOR_0"] = AddAccessor(view, "VEC3", 3, 5121, true);

    auto result = resource::LoadGltf(Write());

    ASSERT_TRUE(result);

    ASSERT_EQ(result->model.meshes.size(), 1U);
    ASSERT_EQ(result->model.meshes[0].primitives.size(), 1U);
    auto const& vertices = result->primitives[0].second.primitiveVertices;
    ASSERT_EQ(vertices.size(), 3U);

    EXPECT_FLOAT_EQ(vertices[0].color.r, 1);
    EXPECT_FLOAT_EQ(vertices[0].color.g, 128.0F / 255.0F);
    EXPECT_FLOAT_EQ(vertices[0].color.b, 0);
    EXPECT_FLOAT_EQ(vertices[0].color.a, 1);
    EXPECT_FLOAT_EQ(vertices[1].color.b, 64.0F / 255.0F);
    EXPECT_FLOAT_EQ(vertices[2].position.y, 1);
}

TEST_F(GltfLoaderTest, DecodesNormalizedShortRgbaAndUvs)
{
    std::vector<std::uint8_t> data;
    for (auto value : {65535U, 32768U, 0U, 16384U, 0U, 65535U, 32768U, 65535U, 16384U, 0U, 65535U, 0U})
    {
        AppendUnsigned(data, value, 2);
    }
    auto view                            = AddView(data);
    Primitive()["attributes"]["COLOR_0"] = AddAccessor(view, "VEC4", 3, 5123, true);
    data.clear();
    for (auto value : {0U, 65535U, 32768U, 16384U, 65535U, 0U})
    {
        AppendUnsigned(data, value, 2);
    }
    view                                    = AddView(data);
    Primitive()["attributes"]["TEXCOORD_0"] = AddAccessor(view, "VEC2", 3, 5123, true);

    auto result = resource::LoadGltf(Write());

    ASSERT_TRUE(result);

    ASSERT_EQ(result->model.meshes.size(), 1U);
    ASSERT_EQ(result->model.meshes[0].primitives.size(), 1U);
    auto const& vertices = result->primitives[0].second.primitiveVertices;
    ASSERT_EQ(vertices.size(), 3U);

    EXPECT_FLOAT_EQ(vertices[0].color.g, 32768.0F / 65535.0F);
    EXPECT_FLOAT_EQ(vertices[0].color.a, 16384.0F / 65535.0F);
    EXPECT_FLOAT_EQ(vertices[2].color.a, 0);
    EXPECT_EQ(vertices[0].texCoord, glm::vec2(0, 0));
    EXPECT_FLOAT_EQ(vertices[1].texCoord.x, 32768.0F / 65535.0F);
    EXPECT_FLOAT_EQ(vertices[1].texCoord.y, 1.0F - 16384.0F / 65535.0F);
}

TEST_F(GltfLoaderTest, LoadsAllUnsignedIndexWidths)
{
    struct IndexFormat
    {
        unsigned width;
        int      componentType;
    };

    constexpr std::array formats{
        IndexFormat{1U, 5121},
        IndexFormat{2U, 5123},
        IndexFormat{4U, 5125},
    };

    std::vector<std::uint32_t> const expected{0, 2, 1, 1, 2, 0};
    for (auto const [width, componentType] : formats)
    {
        SCOPED_TRACE(width);

        std::vector<std::uint8_t> data(4, 255);
        for (auto index : expected)
        {
            AppendUnsigned(data, index, width);
        }
        auto view              = AddView(data);
        Primitive()["indices"] = AddAccessor(view, "SCALAR", 6, componentType, false, 4);

        auto result = resource::LoadGltf(Write());
        ASSERT_TRUE(result);
        ASSERT_EQ(result->model.meshes.size(), 1U);
        ASSERT_EQ(result->model.meshes[0].primitives.size(), 1U);
        auto const& part = result->primitives[0].second;
        EXPECT_EQ(part.primitiveIndices, expected);
        EXPECT_EQ(part.primitiveVertices.size(), 3U);
    }
}

TEST_F(GltfLoaderTest, KeepsIndexedQuadVerticesInAccessorOrder)
{
    auto view                               = AddView(Floats({0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0}));
    auto position                           = AddAccessor(view, "VEC3", 4, 5126);
    document["accessors"][position]["min"]  = {0, 0, 0};
    document["accessors"][position]["max"]  = {1, 1, 0};
    Primitive()["attributes"]["POSITION"]   = position;
    view                                    = AddView(Floats({0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}));
    Primitive()["attributes"]["NORMAL"]     = AddAccessor(view, "VEC3", 4, 5126);
    view                                    = AddView(Floats({0, 0, 1, 0, 1, 1, 0, 1}));
    Primitive()["attributes"]["TEXCOORD_0"] = AddAccessor(view, "VEC2", 4, 5126);
    view                                    = AddView({255, 0, 0, 0, 0, 255, 0, 0, 0, 0, 255, 0, 255, 255, 255, 0}, 4);
    Primitive()["attributes"]["COLOR_0"]    = AddAccessor(view, "VEC3", 4, 5121, true);
    std::vector<std::uint8_t> indices;
    for (auto index : {0U, 2U, 1U, 0U, 3U, 2U})
    {
        AppendUnsigned(indices, index, 2);
    }
    view                   = AddView(indices);
    Primitive()["indices"] = AddAccessor(view, "SCALAR", 6, 5123);

    auto result = resource::LoadGltf(Write());
    ASSERT_TRUE(result);
    auto const& part     = result->primitives[0].second;
    auto const& vertices = part.primitiveVertices;
    EXPECT_EQ(part.primitiveIndices, (std::vector<std::uint32_t>{0, 2, 1, 0, 3, 2}));
    ASSERT_EQ(vertices.size(), 4U);
    std::array const expectedUvs{glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1)};
    std::array const expectedColors{glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), glm::vec4(0, 0, 1, 1), glm::vec4(1, 1, 1, 1)};
    for (std::size_t i{0}; i < vertices.size(); ++i)
    {
        EXPECT_EQ(vertices[i].position, glm::vec3(expectedUvs[i], 0));
        EXPECT_EQ(vertices[i].texCoord, glm::vec2(expectedUvs[i].x, 1.0F - expectedUvs[i].y));
        EXPECT_EQ(vertices[i].color, expectedColors[i]);
        EXPECT_EQ(vertices[i].normal, glm::vec3(0, 0, 1));
        EXPECT_FLOAT_EQ(vertices[i].tangent.w, 1);
    }
}

TEST_F(GltfLoaderTest, DecodesNormalizedByteUvsAndFloatRgb)
{
    auto view                               = AddView({0, 255, 0, 0, 128, 0, 0, 0, 255, 128, 0, 0}, 4);
    Primitive()["attributes"]["TEXCOORD_0"] = AddAccessor(view, "VEC2", 3, 5121, true);
    view                                    = AddView(Floats({1, 0.5F, 0, 0, 1, 0.25F, 0.25F, 0, 1}));
    Primitive()["attributes"]["COLOR_0"]    = AddAccessor(view, "VEC3", 3, 5126);

    auto result = resource::LoadGltf(Write());
    ASSERT_TRUE(result);
    auto const& vertices = result->primitives[0].second.primitiveVertices;
    ASSERT_EQ(vertices.size(), 3U);
    EXPECT_EQ(vertices[0].texCoord, glm::vec2(0, 0));
    EXPECT_FLOAT_EQ(vertices[1].texCoord.x, 128.0F / 255.0F);
    EXPECT_EQ(vertices[0].color, glm::vec4(1, 0.5F, 0, 1));
    EXPECT_EQ(vertices[1].color, glm::vec4(0, 1, 0.25F, 1));
}

TEST_F(GltfLoaderTest, LoadsMultipleMeshesAndMaterialsWithoutTextures)
{
    document["meshes"][0]["name"] = "Sword";
    document["materials"]         = Json::array(
        {{{"alphaMode", "MASK"}, {"alphaCutoff", 0.25}, {"emissiveFactor", {0.1, 0.2, 0.3}},
            {"pbrMetallicRoughness", {{"baseColorFactor", {0.2, 0.3, 0.4, 0.5}}, {"metallicFactor", 0.25}, {"roughnessFactor", 0.75}}}}});
    auto primitive        = Primitive();
    primitive["material"] = 0;
    document["meshes"][0]["primitives"].push_back(primitive);
    document["meshes"].push_back({{"name", "second"}, {"primitives", Json::array({primitive})}});

    auto result = resource::LoadGltf(Write("material.Sword"));
    ASSERT_TRUE(result);
    ASSERT_EQ(result->model.meshes.size(), 2U);
    ASSERT_EQ(result->model.meshes[0].primitives.size(), 2U);
    ASSERT_EQ(result->model.meshes[1].primitives.size(), 1U);
    ASSERT_EQ(result->primitives.size(), 3U);
    EXPECT_EQ(result->model.meshes[0].primitives[0], result->primitives[0].first);
    EXPECT_EQ(result->model.meshes[0].primitives[1], result->primitives[1].first);
    EXPECT_EQ(result->model.meshes[1].primitives[0], result->primitives[2].first);
    EXPECT_EQ(result->model.meshes[1].name, "mesh.second");
    EXPECT_EQ(result->primitives[0].second.material, polos::rendering::kDefaultMaterial);
    EXPECT_EQ(result->primitives[1].second.material, "material.Sword"_sid);
    EXPECT_EQ(result->primitives[2].second.material, "material.Sword"_sid);
    EXPECT_EQ(result->primitives[2].second.primitiveVertices.size(), 3U);
    EXPECT_TRUE(result->materialTextures.empty());
}

TEST_F(GltfLoaderTest, RejectsMismatchedAttributeCountsInEitherDirection)
{
    for (auto count : {2U, 4U})
    {
        SCOPED_TRACE(count);
        auto view                           = AddView(std::vector<std::uint8_t>(count * 12U));
        Primitive()["attributes"]["NORMAL"] = AddAccessor(view, "VEC3", count, 5126);
        EXPECT_FALSE(resource::LoadGltf(Write()));
    }
}

TEST_F(GltfLoaderTest, RejectsAttributesWiderThanTheVertexLayout)
{
    auto view                               = AddView(Floats({0, 0, 1, 0, 1, 0, 1, 0, 0}));
    Primitive()["attributes"]["TEXCOORD_0"] = AddAccessor(view, "VEC3", 3, 5126);
    EXPECT_FALSE(resource::LoadGltf(Write()));
}

TEST_F(GltfLoaderTest, RejectsMissingPositionsAndUnsupportedModes)
{
    auto const original       = document;
    Primitive()["attributes"] = {{"NORMAL", 0}};
    EXPECT_FALSE(resource::LoadGltf(Write()));
    for (auto mode : {0, 1, 5, 6})
    {
        document            = original;
        Primitive()["mode"] = mode;
        EXPECT_FALSE(resource::LoadGltf(Write()));
    }
}

TEST_F(GltfLoaderTest, RecordsExternalTextureIdsAndRejectsInvalidMaterials)
{
    document["images"]                      = Json::array({{{"uri", "textures/missing.png"}}});
    document["textures"]                    = Json::array({{{"source", 0}}});
    document["materials"]                   = Json::array({{{"pbrMetallicRoughness", {{"baseColorTexture", {{"index", 0}}}}}}});
    Primitive()["material"]                 = 0;
    auto view                               = AddView(Floats({0, 0, 1, 0, 0, 1}));
    Primitive()["attributes"]["TEXCOORD_0"] = AddAccessor(view, "VEC2", 3, 5126);

    auto result = resource::LoadGltf(Write("material.test"));
    ASSERT_TRUE(result);
    ASSERT_TRUE(result->materialTextures.contains("material.test"_sid));
    EXPECT_EQ(result->materialTextures.at("material.test"_sid).image, "texture.missing"_sid);

    document["materials"] = Json::array({{{"alphaMode", "INVALID"}}});
    EXPECT_FALSE(resource::LoadGltf(Write("material.test")));
}

TEST_F(GltfLoaderTest, ResolvesRelativeImagesAndSharedSamplerIndices)
{
    std::string const filename              = "sampler.png";
    document["images"]                      = Json::array({{{"uri", "textures/" + filename}}});
    document["samplers"]                    = Json::array({{{"minFilter", 9986}, {"magFilter", 9729}, {"wrapS", 33071}, {"wrapT", 33648}}});
    document["textures"]                    = Json::array({{{"source", 0}, {"sampler", 0}}, {{"source", 0}, {"sampler", 0}}});
    document["materials"]                   = Json::array({{{"pbrMetallicRoughness", {{"baseColorTexture", {{"index", 1}}}}}}});
    Primitive()["material"]                 = 0;
    auto view                               = AddView(Floats({0, 0, 1, 0, 0, 1}));
    Primitive()["attributes"]["TEXCOORD_0"] = AddAccessor(view, "VEC2", 3, 5126);

    auto result = resource::LoadGltf(Write("material.test"));
    ASSERT_TRUE(result);
    ASSERT_TRUE(result->materialTextures.contains("material.test"_sid));
    auto const& texture = result->materialTextures.at("material.test"_sid);
    EXPECT_EQ(texture.image, "texture.sampler"_sid);
    EXPECT_EQ(texture.sampler.minFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(texture.sampler.magFilter, VK_FILTER_LINEAR);
    EXPECT_EQ(texture.sampler.mipmap, VK_SAMPLER_MIPMAP_MODE_LINEAR);
    EXPECT_EQ(texture.sampler.addressModeU, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    EXPECT_EQ(texture.sampler.addressModeV, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
}

TEST_F(GltfLoaderTest, RejectsMalformedAccessorsAndRanges)
{
    auto const original               = document;
    auto       expectRejectedDocument = [&](auto mutate) {
        document = original;
        mutate();
        EXPECT_FALSE(resource::LoadGltf(Write())) << document.dump();
    };

    expectRejectedDocument([&] {
        Primitive()["attributes"]["POSITION"] = -1;
    });

    expectRejectedDocument([&] {
        Primitive()["attributes"]["POSITION"] = 100;
    });

    expectRejectedDocument([&] {
        Primitive()["attributes"].erase("POSITION");
    });

    expectRejectedDocument([&] {
        document["accessors"][0].erase("bufferView");
    });

    expectRejectedDocument([&] {
        document["accessors"][0]["bufferView"] = 100;
    });

    expectRejectedDocument([&] {
        document["bufferViews"][0]["buffer"] = 100;
    });

    expectRejectedDocument([&] {
        document["bufferViews"][0]["byteLength"] = bytes.size() + 1;
    });

    expectRejectedDocument([&] {
        document["accessors"][0]["byteOffset"] = 40;
    });

    expectRejectedDocument([&] {
        document["accessors"][0]["byteOffset"] = 4;
    });

    expectRejectedDocument([&] {
        document["accessors"][0]["count"] = std::numeric_limits<std::uint64_t>::max();
    });

    expectRejectedDocument([&] {
        document["accessors"][0]["type"] = "MAT4";
    });

    expectRejectedDocument([&] {
        document["accessors"][0]["componentType"] = 9999;
    });

    expectRejectedDocument([&] {
        document["accessors"][0]["normalized"] = true;
    });

    expectRejectedDocument([&] {
        document["accessors"][0]["sparse"] = {{"count", 1}, {"indices", {{"bufferView", 0}, {"componentType", 5121}}}, {"values", {{"bufferView", 0}}}};
    });

    expectRejectedDocument([&] {
        auto accessor     = document["accessors"][0];
        accessor["count"] = 2;
        document["accessors"].push_back(accessor);
        Primitive()["attributes"]["NORMAL"] = 1;
    });
}

TEST_F(GltfLoaderTest, ReturnsNullForMissingOrMalformedFiles)
{
    EXPECT_EQ(resource::LoadGltf(directory / "missing.gltf"), nullptr);

    auto const descriptor = Write();
    std::ofstream(directory / "mesh.gltf") << "{invalid json";
    EXPECT_EQ(resource::LoadGltf(descriptor), nullptr);
}

TEST_F(GltfLoaderTest, SkipsUnsupportedAttributesAndCleansUpFailedLoads)
{
    Primitive()["attributes"]["_UNUSED"] = 0;

    EXPECT_TRUE(resource::LoadGltf(Write()));

    document["accessors"][0]["count"] = 100;

    EXPECT_FALSE(resource::LoadGltf(Write()));

    document["accessors"][0]["count"] = 3;
    auto path                         = Write();

    auto first  = std::async(std::launch::async, [path] {
        return resource::LoadGltf(path);
    });
    auto second = std::async(std::launch::async, [path] {
        return resource::LoadGltf(path);
    });

    ASSERT_TRUE(first.get());
    ASSERT_TRUE(second.get());
}
}// namespace
