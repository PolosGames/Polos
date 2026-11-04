#include "polos/resource/gltf/gltf_loader.hpp"
#include "polos/resource/model.hpp"

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
                    ("polos-gltf-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
                     std::to_string(serial++));
        std::filesystem::create_directory(directory);
        document = {
            {"asset", {{"version", "2.0"}}},
            {"buffers", Json::array()},
            {"bufferViews", Json::array()},
            {"accessors", Json::array()},
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

    auto AddAccessor(
        std::size_t        view,
        std::string const& type,
        unsigned           count,
        int                component,
        bool               normalized = false,
        unsigned           offset     = 0) -> std::size_t
    {
        auto index = document["accessors"].size();
        document["accessors"].push_back(
            {{"bufferView", view},
             {"byteOffset", offset},
             {"type", type},
             {"componentType", component},
             {"count", count},
             {"normalized", normalized}});
        return index;
    }

    auto Write() -> std::string
    {
        document["buffers"] = Json::array({{{"uri", "mesh.bin"}, {"byteLength", bytes.size()}}});
        std::ofstream binary(directory / "mesh.bin", std::ios::binary);
        binary.write(reinterpret_cast<char const*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        std::ofstream json(directory / "mesh.gltf");
        json << document.dump();
        return (directory / "mesh.gltf").string();
    }
};

TEST_F(GltfLoaderTest, LoadsPackedPositionsAndDefaultsWithoutIndices)
{
    auto result = resource::LoadGltf(Write());

    ASSERT_TRUE(result);

    auto const& model = *result;

    ASSERT_EQ(model.streams.size(), 1U);

    ASSERT_EQ(model.meshes.size(), 1U);
    ASSERT_EQ(model.meshes[0].primitives.size(), 1U);

    auto const& primitive = model.meshes[0].primitives[0];

    EXPECT_EQ(primitive.attributes, 0U);
    EXPECT_EQ(primitive.indices, std::numeric_limits<std::size_t>::max());
    EXPECT_TRUE(model.indexStreams.empty());

    auto const& vertices = model.streams[primitive.attributes].data;

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

    ASSERT_EQ(result->streams.size(), 1U);
    auto const& vertices = result->streams[0].data;
    ASSERT_EQ(vertices.size(), 3U);

    EXPECT_EQ(vertices[0].normal, glm::vec3(0, 0, 1));
    EXPECT_EQ(vertices[1].normal, glm::vec3(0, 1, 0));
    EXPECT_EQ(vertices[2].normal, glm::vec3(1, 0, 0));
    EXPECT_EQ(vertices[1].texCoord, glm::vec2(1, 0));
    EXPECT_EQ(vertices[2].texCoord, glm::vec2(0, 1));
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

    ASSERT_EQ(result->streams.size(), 1U);
    auto const& vertices = result->streams[0].data;
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

    ASSERT_EQ(result->streams.size(), 1U);
    auto const& vertices = result->streams[0].data;
    ASSERT_EQ(vertices.size(), 3U);

    EXPECT_FLOAT_EQ(vertices[0].color.g, 32768.0F / 65535.0F);
    EXPECT_FLOAT_EQ(vertices[0].color.a, 16384.0F / 65535.0F);
    EXPECT_FLOAT_EQ(vertices[2].color.a, 0);
    EXPECT_EQ(vertices[0].texCoord, glm::vec2(0, 1));
    EXPECT_FLOAT_EQ(vertices[1].texCoord.x, 32768.0F / 65535.0F);
    EXPECT_FLOAT_EQ(vertices[1].texCoord.y, 16384.0F / 65535.0F);
}

TEST_F(GltfLoaderTest, LoadsAllUnsignedIndexWidthsAndRejectsOutOfRangeIndices)
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

    for (auto const [width, componentType] : formats)
    {
        SCOPED_TRACE(width);

        std::vector<std::uint8_t> data;
        for (auto value : {0U, 2U, 1U})
        {
            AppendUnsigned(data, value, width);
        }
        auto view              = AddView(data);
        Primitive()["indices"] = AddAccessor(view, "SCALAR", 3, componentType);

        auto result = resource::LoadGltf(Write());
        ASSERT_TRUE(result) << width;
        ASSERT_EQ(result->indexStreams.size(), 1U);
        ASSERT_EQ(result->meshes.size(), 1U);
        ASSERT_EQ(result->meshes[0].primitives.size(), 1U);
        EXPECT_EQ(
            result->indexStreams[result->meshes[0].primitives[0].indices].data,
            (std::vector<std::uint32_t>{0, 2, 1}));
    }

    bytes.back() = 255;// Corrupt the last UINT32 index.
    EXPECT_FALSE(resource::LoadGltf(Write()));
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
        document["accessors"][0]["sparse"] =
            {{"count", 1}, {"indices", {{"bufferView", 0}, {"componentType", 5121}}}, {"values", {{"bufferView", 0}}}};
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

    auto path = directory / "malformed.gltf";
    {
        std::ofstream file(path);
        file << "{invalid json";
    }

    EXPECT_EQ(resource::LoadGltf(path), nullptr);
}

TEST_F(GltfLoaderTest, SkipsUnsupportedAttributesAndCleansUpFailedLoads)
{
    Primitive()["attributes"]["_UNUSED"] = 999;

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
