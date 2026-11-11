#include "polos/resource/gltf/gltf_loader.hpp"
#include "polos/resource/model.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>

namespace
{
class GltfValidationTest : public ::testing::Test
{
protected:
    std::filesystem::path directory;
    nlohmann::json        document;

    void SetUp() override
    {
        static std::atomic<unsigned> serial{};
        directory =
            std::filesystem::temp_directory_path() /
            ("polos-gltf-validation-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
             "-" + std::to_string(serial++));
        std::filesystem::create_directory(directory);
        document = {
            {"asset", {{"version", "2.0"}}},
            {"buffers", {{{"uri", "mesh.bin"}, {"byteLength", 36}}}},
            {"bufferViews", {{{"buffer", 0}, {"byteLength", 36}}}},
            {"accessors",
             {{{"bufferView", 0},
               {"componentType", 5126},
               {"type", "VEC3"},
               {"count", 3},
               {"min", {0, 0, 0}},
               {"max", {0, 0, 0}}}}},
            {"meshes", {{{"name", "triangle"}, {"primitives", {{{"attributes", {{"POSITION", 0}}}}}}}}},
        };
    }

    void TearDown() override
    {
        std::filesystem::remove_all(directory);
    }

    auto Load() -> std::unique_ptr<polos::resource::model::Model>
    {
        {
            std::array<char, 36> bytes{};
            std::ofstream        binary(directory / "mesh.bin", std::ios::binary);
            binary.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
            std::ofstream json(directory / "mesh.gltf");
            json << document.dump();
        }
        return polos::resource::model::LoadGltf(directory / "mesh.gltf");
    }
};

TEST_F(GltfValidationTest, AcceptsValidLoadedBuffer)
{
    ASSERT_NE(Load(), nullptr);
}

TEST_F(GltfValidationTest, RejectsInvalidReferences)
{
    document["accessors"][0]["bufferView"] = 100;
    EXPECT_EQ(Load(), nullptr);

    document["accessors"][0]["bufferView"] = 0;
    document["bufferViews"][0]["buffer"]   = -1;
    EXPECT_EQ(Load(), nullptr);

    document["bufferViews"][0]["buffer"] = 100;
    EXPECT_EQ(Load(), nullptr);

    document["bufferViews"][0]["buffer"]                            = 0;
    document["meshes"][0]["primitives"][0]["attributes"]["_UNUSED"] = 100;
    EXPECT_EQ(Load(), nullptr);
}

TEST_F(GltfValidationTest, RejectsInvalidByteRanges)
{
    document["bufferViews"][0]["byteLength"] = 37;
    EXPECT_EQ(Load(), nullptr);

    document["bufferViews"][0]["byteOffset"] = std::numeric_limits<std::uint64_t>::max() - 4095U;
    document["bufferViews"][0]["byteLength"] = 8192;
    EXPECT_EQ(Load(), nullptr);

    document["bufferViews"][0]["byteOffset"] = 0;
    document["bufferViews"][0]["byteLength"] = 36;
    document["accessors"][0]["byteOffset"]   = 40;
    EXPECT_EQ(Load(), nullptr);

    document["accessors"][0]["byteOffset"] = 4;
    EXPECT_EQ(Load(), nullptr);

    document["accessors"][0]["byteOffset"] = 0;
    document["accessors"][0]["count"]      = std::numeric_limits<std::uint64_t>::max();
    EXPECT_EQ(Load(), nullptr);

    document["accessors"][0]["count"]        = 3;
    document["bufferViews"][0]["byteStride"] = 8;
    EXPECT_EQ(Load(), nullptr);
}

TEST_F(GltfValidationTest, RejectsSpecViolationsInStrictMode)
{
    document["accessors"][0]["count"] = 0;
    EXPECT_EQ(Load(), nullptr);

    document["accessors"][0]["count"]      = 3;
    document["accessors"][0]["normalized"] = true;
    EXPECT_EQ(Load(), nullptr);

    document["accessors"][0]["normalized"]   = false;
    document["bufferViews"][0]["byteStride"] = 14;
    EXPECT_EQ(Load(), nullptr);
}

TEST_F(GltfValidationTest, RejectsMissingBufferData)
{
    document["buffers"][0].erase("uri");
    EXPECT_EQ(Load(), nullptr);
}

TEST_F(GltfValidationTest, RejectsAccessorWithoutBufferView)
{
    document["accessors"][0].erase("bufferView");
    EXPECT_EQ(Load(), nullptr);
}

TEST_F(GltfValidationTest, RejectsUnsupportedSparseAccessor)
{
    document["accessors"][0]["sparse"] =
        {{"count", 1}, {"indices", {{"bufferView", 0}, {"componentType", 5121}}}, {"values", {{"bufferView", 0}}}};
    EXPECT_EQ(Load(), nullptr);
}
}// namespace
