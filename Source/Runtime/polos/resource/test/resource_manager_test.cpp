#include "polos/filesystem/file.hpp"
#include "polos/rendering/cache/material_cache.hpp"
#include "polos/rendering/cache/model_cache.hpp"
#include "polos/rendering/resources/image_data.hpp"
#include "polos/resource/resource_manager.hpp"

#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <type_traits>
#include <vector>

namespace
{
using polos::resource::ResourceManager;
static_assert(std::is_same_v<decltype(std::declval<ResourceManager&>().LoadCatalog()), void>);

// Accessors accept ResourceId only; strings and filesystem paths cannot select resources.
static_assert(!std::is_invocable_v<decltype(&ResourceManager::GetImage), ResourceManager const&, char const*>);
static_assert(!std::is_invocable_v<decltype(&polos::rendering::ModelCache::GetModel), polos::rendering::ModelCache const&, std::string_view>);
static_assert(!std::is_invocable_v<decltype(&ResourceManager::GetFile), ResourceManager const&, std::filesystem::path>);

class ResourceManagerTest : public ::testing::Test
{
protected:
    std::filesystem::path            directory;
    std::unique_ptr<ResourceManager> resources;
    polos::rendering::ModelCache     models;
    polos::rendering::MaterialCache  materials;

    void SetUp() override
    {
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        static std::atomic<unsigned> serial{};
        directory = std::filesystem::temp_directory_path() /
                    ("polos-resources-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" + std::to_string(serial++));
        std::filesystem::create_directories(directory / "Resources");
        resources = std::make_unique<ResourceManager>(directory / "Resources");
        ASSERT_TRUE(models.Create({}));
        ASSERT_TRUE(materials.Create({}));
    }

    void TearDown() override
    {
        resources.reset();
        std::filesystem::remove_all(directory);
    }

    auto Write(std::string_view path, std::string_view text) -> void
    {
        auto target = directory / "Resources" / path;
        std::filesystem::create_directories(target.parent_path());
        std::ofstream file(target);
        file << text;
    }

    auto Catalog(std::initializer_list<std::pair<std::string_view, std::string_view>> entries) -> void
    {
        if (entries.size() == 0U)
        {
            Write("resources.yaml", "resources: []\n");
        }
        else
        {
            std::string catalog = "resources:\n";
            for (auto const& [id, path] : entries)
            {
                catalog += "  - id: " + std::string(id) + "\n    path: " + std::string(path) + "\n";
            }
            Write("resources.yaml", catalog);
        }
        resources->LoadCatalog();
    }

    auto WriteModel(std::string_view path) -> void
    {
        Write(path, "id: model.test\npath: test.gltf\nmaterials: []\n");
        Write("test.gltf", R"({"asset":{"version":"2.0"},"nodes":[{"name":"Root"}]})");
    }

    auto WriteImage(std::string_view path, unsigned char red) -> void
    {
        // One uncompressed RGBA pixel, top-left origin, BGRA byte order.
        std::array<unsigned char, 18> header{};
        header[2]  = 2;
        header[12] = 1;
        header[14] = 1;
        header[16] = 32;
        header[17] = 0x28;
        std::array<unsigned char, 4> const pixel{0x33, 0x22, red, 0xFF};

        auto target = directory / "Resources" / path;
        std::filesystem::create_directories(target.parent_path());
        std::ofstream file(target, std::ios::binary);
        file.write(reinterpret_cast<char const*>(header.data()), static_cast<std::streamsize>(header.size()));
        file.write(reinterpret_cast<char const*>(pixel.data()), static_cast<std::streamsize>(pixel.size()));
    }
};

TEST_F(ResourceManagerTest, GettersReturnNullBeforeCatalogLoad)
{
    WriteImage("pixel.tga", 0x11);

    EXPECT_EQ(resources->GetImage("pixel"_sid), nullptr);
    EXPECT_EQ(models.GetModel("model.test"_sid), nullptr);
}

TEST_F(ResourceManagerTest, LoadsImagesFromExplicitPathsWithoutCatalog)
{
    WriteImage("pixel.tga", 0x11);
    auto const path = directory / "pixel.tga";
    std::filesystem::rename(directory / "Resources/pixel.tga", path);

    auto image = resources->LoadImage("dynamic.image"_sid, path);

    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->width, 1);
    EXPECT_EQ(image->height, 1);
    EXPECT_EQ(image->components, 4);
    ASSERT_EQ(image->data.size(), 4U);
    EXPECT_EQ(image->data[0], 0x11);
    std::filesystem::remove(path);
    EXPECT_EQ(resources->GetImage("dynamic.image"_sid), image);
}

TEST_F(ResourceManagerTest, LoadsBinaryFilesFromPathsWithoutCatalog)
{
    Write("data.bin", std::string_view("a\0b", 3));
    auto const path = directory / "Resources/data.bin";

    auto file = resources->LoadFile("dynamic.file"_sid, path);

    ASSERT_NE(file, nullptr);
    EXPECT_EQ(resources->GetFile("dynamic.file"_sid), file);
    EXPECT_EQ(file->size, 3U);
    EXPECT_EQ(file->uncompressedSize, 3U);
    EXPECT_EQ(file->path, path);
    EXPECT_EQ(file->stemName, "data");
    EXPECT_EQ(file->customName, "data.bin");
    EXPECT_EQ(file->data, (std::vector<std::byte>{std::byte{0x61}, std::byte{0}, std::byte{0x62}}));
}

TEST_F(ResourceManagerTest, LoadsEmptyFilesFromPaths)
{
    Write("empty.bin", "");
    auto file = resources->LoadFile("empty.path"_sid, directory / "Resources/empty.bin");

    ASSERT_NE(file, nullptr);
    EXPECT_TRUE(file->data.empty());
    EXPECT_EQ(file->size, 0U);
    EXPECT_EQ(file->uncompressedSize, 0U);
}

TEST_F(ResourceManagerTest, FailedExplicitLoadsLeaveIdsAvailableForRetry)
{
    auto const missing = directory / "Resources/pixel.tga";

    EXPECT_EQ(resources->LoadImage("retry.image"_sid, missing), nullptr);
    EXPECT_EQ(resources->GetImage("retry.image"_sid), nullptr);
    EXPECT_EQ(resources->LoadFile("retry.file"_sid, missing), nullptr);
    EXPECT_EQ(resources->GetFile("retry.file"_sid), nullptr);

    WriteImage("pixel.tga", 0x55);
    auto image = resources->LoadImage("retry.image"_sid, missing);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->data[0], 0x55);
    EXPECT_NE(resources->LoadFile("retry.file"_sid, missing), nullptr);
}

TEST_F(ResourceManagerTest, ExplicitLoadsCanCacheBothRepresentationsUnderOneId)
{
    WriteImage("pixel.tga", 0x11);
    auto const path = directory / "Resources/pixel.tga";

    auto image = resources->LoadImage("pixel"_sid, path);
    auto file  = resources->LoadFile("pixel"_sid, path);

    ASSERT_NE(image, nullptr);
    ASSERT_NE(file, nullptr);
    EXPECT_EQ(resources->GetImage("pixel"_sid), image);
    EXPECT_EQ(resources->GetFile("pixel"_sid), file);
    EXPECT_EQ(image->data.size(), 4U);
    EXPECT_EQ(file->size, 22U);
}

TEST_F(ResourceManagerTest, CatalogPreloadsModelsAndMaterialsIntoTheirCaches)
{
    WriteModel("Models/test.yaml");
    Write("Materials/test.yaml", "baseColorFactor: [0.1, 0.2, 0.3, 1.0]\n");
    Catalog({{"model.test", "Models/test.yaml"}, {"material.test", "Materials/test.yaml"}});

    std::filesystem::remove_all(directory / "Resources");
    auto const* model = models.GetModel("model.test"_sid);
    ASSERT_NE(model, nullptr);
    ASSERT_EQ(model->nodes.size(), 1U);
    EXPECT_EQ(model->nodes[0].name, "Root");
    EXPECT_EQ(model->rootNodes, (std::vector<std::size_t>{0U}));

    auto const* material = materials.GetMaterial("material.test"_sid);
    ASSERT_NE(material, nullptr);
    EXPECT_EQ(material->baseColorFactor, glm::vec4(0.1F, 0.2F, 0.3F, 1.0F));
    EXPECT_EQ(material->baseColorTexture, polos::rendering::kDefaultTexture);
}

TEST_F(ResourceManagerTest, RegistersTexturePathsWithoutDecodingUnreferencedImages)
{
    Catalog({{"texture.unused", "missing.png"}});
    EXPECT_EQ(resources->GetImage("texture.unused"_sid), nullptr);
}

TEST_F(ResourceManagerTest, InvalidOrDuplicateIdsAbort)
{
    EXPECT_DEATH(Catalog({{"texture.one", "a.png"}, {"texture.one", "b.png"}}), "");
    EXPECT_DEATH(Catalog({{"", "a.png"}}), "");
}

TEST_F(ResourceManagerTest, SharedReferencesSurviveCacheClearing)
{
    WriteImage("pixel.tga", 0x77);
    Write("data.bin", "data");
    auto const image = resources->LoadImage("pixel"_sid, directory / "Resources/pixel.tga");
    auto const file  = resources->LoadFile("data"_sid, directory / "Resources/data.bin");

    ASSERT_NE(image, nullptr);
    ASSERT_NE(file, nullptr);
    resources->ClearCache();

    EXPECT_EQ(resources->GetImage("pixel"_sid), nullptr);
    EXPECT_EQ(resources->GetFile("data"_sid), nullptr);
    EXPECT_EQ(image->data[0], 0x77);
    EXPECT_EQ(file->data.size(), 4U);
}

TEST_F(ResourceManagerTest, CatalogPreservesModelIdentityAcrossDescriptorMoves)
{
    WriteModel("Models/moved.yaml");
    Catalog({{"model.Sponza", "Models/moved.yaml"}});

    EXPECT_NE(models.GetModel("model.Sponza"_sid), nullptr);
    EXPECT_EQ(models.GetModel("model.test"_sid), nullptr);
}

TEST_F(ResourceManagerTest, MissingOrMalformedCatalogAborts)
{
    EXPECT_DEATH(resources->LoadCatalog(), "");
    for (auto catalog : {"{broken", "{}", "resources: {}\n", "resources:\n  - id: texture.test\n"})
    {
        SCOPED_TRACE(catalog);
        Write("resources.yaml", catalog);
        EXPECT_DEATH(resources->LoadCatalog(), "");
    }
}

TEST_F(ResourceManagerTest, MissingModelOrMaterialAbortsCatalogLoading)
{
    EXPECT_DEATH(Catalog({{"model.test", "Models/missing.yaml"}}), "");
    EXPECT_DEATH(Catalog({{"material.test", "Materials/missing.yaml"}}), "");
}

TEST_F(ResourceManagerTest, UnavailableResourceRootAbortsCatalogLoading)
{
    ResourceManager unavailable(std::filesystem::path{});

    EXPECT_DEATH(unavailable.LoadCatalog(), "");
}

TEST(ResourceManagerRootTest, DefaultRootIsResourcesBesideExecutable)
{
    ResourceManager resources;
#ifdef __linux__
    EXPECT_EQ(resources.Root(), std::filesystem::read_symlink("/proc/self/exe").parent_path() / "Resources");
#endif
    EXPECT_EQ(resources.Root().filename(), "Resources");
}
}// namespace
