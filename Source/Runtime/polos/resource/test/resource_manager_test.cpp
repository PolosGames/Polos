#include "polos/filesystem/file.hpp"
#include "polos/resource/image.hpp"
#include "polos/resource/model.hpp"
#include "polos/resource/resource_manager.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <type_traits>

namespace
{
using polos::resource::ResourceManager;
using Json = nlohmann::json;
static_assert(std::is_same_v<decltype(std::declval<ResourceManager&>().LoadCatalog()), void>);

// Accessors accept ResourceId only; strings and filesystem paths cannot select resources.
static_assert(!std::is_invocable_v<decltype(&ResourceManager::GetImage), ResourceManager const&, char const*>);
static_assert(!std::is_invocable_v<decltype(&ResourceManager::GetModel), ResourceManager const&, std::string_view>);
static_assert(!std::is_invocable_v<decltype(&ResourceManager::GetFile), ResourceManager const&, std::filesystem::path>);

class ResourceManagerTest : public ::testing::Test
{
protected:
    std::filesystem::path            directory;
    std::unique_ptr<ResourceManager> resources;

    void SetUp() override
    {
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        static std::atomic<unsigned> serial{};
        directory = std::filesystem::temp_directory_path() /
                    ("polos-resources-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
                     "-" + std::to_string(serial++));
        std::filesystem::create_directories(directory / "Resources");
        resources = std::make_unique<ResourceManager>(directory / "Resources");
    }

    void TearDown() override
    {
        resources.reset();
        std::filesystem::remove_all(directory);
    }

    void Write(std::string_view path, std::string_view text)
    {
        auto target = directory / "Resources" / path;
        std::filesystem::create_directories(target.parent_path());
        std::ofstream file(target);
        file << text;
    }

    auto Catalog(std::initializer_list<std::pair<std::string_view, std::string_view>> entries) -> void
    {
        auto catalog = Json::array();
        for (auto const& [id, path] : entries) catalog.push_back({{"id", id}, {"path", path}});
        Write("resources.json", catalog.dump());
        resources->LoadCatalog();
    }

    void WriteImage(std::string_view path, unsigned char red)
    {
        // One uncompressed RGBA pixel, top-left origin, BGRA byte order.
        std::array<unsigned char, 22> const
             tga{0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 32, 0x28, 0x33, 0x22, red, 0xFF};
        auto target = directory / "Resources" / path;
        std::filesystem::create_directories(target.parent_path());
        std::ofstream file(target, std::ios::binary);
        file.write(reinterpret_cast<char const*>(tga.data()), static_cast<std::streamsize>(tga.size()));
    }
};

TEST_F(ResourceManagerTest, GettersReturnNullBeforeCatalogLoad)
{
    WriteImage("pixel.tga", 0x11);
    EXPECT_EQ(resources->GetImage("pixel"_sid), nullptr);
    EXPECT_EQ(resources->GetModel("model"_sid), nullptr);
}

TEST_F(ResourceManagerTest, GettersDoNotLoadOtherRepresentations)
{
    WriteImage("pixel.tga", 0x11);
    WriteImage("encoded.bin", 0x99);
    Write("model.gltf", R"({"asset":{"version":"2.0"},"scenes":[{}]})");
    Catalog({{"pixel", "pixel.tga"}, {"encoded", "encoded.bin"}, {"model", "model.gltf"}});
    ResourceManager const& view = *resources;
    EXPECT_NE(view.GetImage("pixel"_sid), nullptr);
    EXPECT_NE(view.GetFile("encoded"_sid), nullptr);
    EXPECT_NE(view.GetModel("model"_sid), nullptr);
    // The files exist, but getters must not read or decode an additional representation.
    EXPECT_EQ(view.GetFile("pixel"_sid), nullptr);
    EXPECT_EQ(view.GetImage("encoded"_sid), nullptr);
    EXPECT_EQ(view.GetModel("encoded"_sid), nullptr);
    EXPECT_EQ(view.GetFile("model"_sid), nullptr);
}

TEST_F(ResourceManagerTest, ClearedCachesStayEmptyUntilExplicitCatalogReload)
{
    WriteImage("pixel.tga", 0x11);
    Write("model.gltf", R"({"asset":{"version":"2.0"},"scenes":[{}]})");
    Write("data.bin", "data");
    Catalog({{"pixel", "pixel.tga"}, {"model", "model.gltf"}, {"data", "data.bin"}});
    resources->ClearCache();
    // All files remain available on disk. Access must still return nullptr.
    EXPECT_EQ(resources->GetImage("pixel"_sid), nullptr);
    EXPECT_EQ(resources->GetModel("model"_sid), nullptr);
    EXPECT_EQ(resources->GetFile("data"_sid), nullptr);
    resources->LoadCatalog();
    EXPECT_NE(resources->GetImage("pixel"_sid), nullptr);
    EXPECT_NE(resources->GetModel("model"_sid), nullptr);
    EXPECT_NE(resources->GetFile("data"_sid), nullptr);
}

TEST_F(ResourceManagerTest, CatalogPreloadsEveryResourceAndServesCachedDataAfterFilesAreRemoved)
{
    WriteImage("Textures/pixel.TGA", 0x11);
    Write("Models/empty.GLTF", R"({"asset":{"version":"2.0"},"scenes":[{}]})");
    Write("Shaders/quad.vert.spv", "shader bytes");
    Write("data.json", "{}");
    Catalog(
        {{"pixel", "Textures/pixel.TGA"},
         {"model", "Models/empty.GLTF"},
         {"shader", "Shaders/quad.vert.spv"},
         {"data", "data.json"}});

    // No load accessor has been called yet. All four representations must already be cached.
    std::filesystem::remove_all(directory / "Resources");
    auto image  = resources->GetImage("pixel"_sid);
    auto model  = resources->GetModel("model"_sid);
    auto shader = resources->GetFile("shader"_sid);
    auto data   = resources->GetFile("data"_sid);
    ASSERT_TRUE(image);
    ASSERT_TRUE(model);
    ASSERT_TRUE(shader);
    ASSERT_TRUE(data);
    EXPECT_EQ(image->data[0], 0x11);
    EXPECT_EQ(shader->size, 12U);
    EXPECT_EQ(data->size, 2U);
}

TEST_F(ResourceManagerTest, ResourcesWithSameFilenameAreDistinctById)
{
    WriteImage("characters/tux.png", 0x11);
    WriteImage("icons/tux.png", 0x99);
    Catalog({{"texture.character", "characters/tux.png"}, {"texture.icon", "icons/tux.png"}});
    auto character = resources->GetImage("texture.character"_sid);
    auto icon      = resources->GetImage("texture.icon"_sid);
    ASSERT_TRUE(character);
    ASSERT_TRUE(icon);
    EXPECT_NE(character, icon);
    EXPECT_EQ(character->data[0], 0x11);
    EXPECT_EQ(icon->data[0], 0x99);
    // Hashing a path or filename does not resolve it to the registered resource's ID.
    EXPECT_EQ(resources->GetImage("characters/tux.png"_sid), nullptr);
    EXPECT_EQ(resources->GetImage("tux.png"_sid), nullptr);
}

TEST_F(ResourceManagerTest, CatalogNormalizesPathsBeforeLoading)
{
    WriteImage("characters/tux.png", 0x11);
    WriteImage("Characters/Tux.png", 0x22);
    Catalog({{"texture.character", "characters\\.\\tux.png"}, {"texture.upper", "Characters/../Characters/Tux.png"}});
    auto character = resources->GetImage("texture.character"_sid);
    auto upper     = resources->GetImage("texture.upper"_sid);
    ASSERT_TRUE(character);
    ASSERT_TRUE(upper);
    EXPECT_EQ(character->data[0], 0x11);
    EXPECT_EQ(upper->data[0], 0x22);
}

TEST_F(ResourceManagerTest, InvalidIdsAndDuplicateIdsOrNormalizedPathsAbort)
{
    Write("a.bin", "data");
    Write("b.bin", "data");
    EXPECT_DEATH(Catalog({{"one", "a.bin"}, {"two", "./a.bin"}}), "");
    EXPECT_DEATH(Catalog({{"one", "a.bin"}, {"one", "b.bin"}}), "");
    for (auto id : {std::string_view{}, std::string_view("a\0b", 3)}) { EXPECT_DEATH(Catalog({{id, "a.bin"}}), ""); }
}

TEST_F(ResourceManagerTest, EscapingSymlinksAbortCatalogLoading)
{
    Write("safe/data.txt", "inside");
    std::filesystem::create_directory(directory / "outside");
    std::error_code error;
    std::filesystem::create_directory_symlink(directory / "outside", directory / "Resources/link", error);
    if (error)
        GTEST_SKIP() << "Symlinks unavailable: " << error.message();
    EXPECT_DEATH(Catalog({{"escape", "link/data.txt"}}), "");
    Catalog({{"safe", "safe/data.txt"}});
    auto original = resources->GetFile("safe"_sid);
    std::filesystem::remove_all(directory / "Resources/safe");
    std::filesystem::create_directory_symlink(directory / "outside", directory / "Resources/safe");
    EXPECT_DEATH(resources->LoadCatalog(), "");
    // Disk changes cannot invalidate a preloaded resource or trigger a fresh read.
    EXPECT_NE(resources->GetFile("safe"_sid), nullptr);
    EXPECT_EQ(resources->GetFile("safe"_sid), original);
}

TEST_F(ResourceManagerTest, SharedReferencesSurviveCacheClearingAndCatalogReload)
{
    WriteImage("pixel.tga", 0x77);
    Catalog({{"pixel", "pixel.tga"}});
    auto original = resources->GetImage("pixel"_sid);
    ASSERT_TRUE(original);
    resources->ClearCache();
    EXPECT_EQ(resources->GetImage("pixel"_sid), nullptr);
    EXPECT_EQ(original->data[0], 0x77);
    WriteImage("pixel.tga", 0x99);
    resources->LoadCatalog();
    auto replacement = resources->GetImage("pixel"_sid);
    ASSERT_TRUE(replacement);
    EXPECT_NE(original, replacement);
    EXPECT_EQ(replacement->data[0], 0x99);
    EXPECT_EQ(original->data[0], 0x77);
}

TEST_F(ResourceManagerTest, UnlistedFilesCannotBeRetrieved)
{
    WriteImage("listed.tga", 0x11);
    WriteImage("unlisted.tga", 0x99);
    Catalog({{"listed", "listed.tga"}});
    EXPECT_EQ(resources->GetImage("unlisted"_sid), nullptr);
}

TEST_F(ResourceManagerTest, CatalogPreservesIdentityAcrossMovesAndManagerRestarts)
{
    Write("old/data.txt", "hello");
    Catalog({{"stable", "old/data.txt"}});
    auto original = resources->GetFile("stable"_sid);
    ASSERT_TRUE(original);
    std::filesystem::create_directory(directory / "Resources/new");
    std::filesystem::rename(directory / "Resources/old/data.txt", directory / "Resources/new/renamed.txt");
    Write("resources.json", R"([{"id":"stable","path":"new/renamed.txt"}])");
    resources = std::make_unique<ResourceManager>(directory / "Resources");
    resources->LoadCatalog();
    auto moved = resources->GetFile("stable"_sid);
    ASSERT_TRUE(moved);
    EXPECT_EQ(original->data, moved->data);
}

TEST_F(ResourceManagerTest, MissingOrMalformedCatalogAborts)
{
    EXPECT_DEATH(resources->LoadCatalog(), "");
    for (auto catalog :
         {"{broken", "{}", "[{}]", R"([{"id":123,"path":"a"}])", R"([{"id":"escape","path":"../outside"}])"})
    {
        Write("resources.json", catalog);
        EXPECT_DEATH(resources->LoadCatalog(), "");
    }
}

TEST_F(ResourceManagerTest, MissingOrMalformedResourcesAbortImmediately)
{
    for (auto missing : {"missing.tga", "missing.gltf", "missing.spv"})
    {
        EXPECT_DEATH(Catalog({{"missing", missing}}), "");
    }
    Write("invalid.png", "not an image");
    EXPECT_DEATH(Catalog({{"invalid", "invalid.png"}}), "");
    Write("invalid.gltf", "not glTF");
    EXPECT_DEATH(Catalog({{"invalid", "invalid.gltf"}}), "");
}

TEST_F(ResourceManagerTest, UnavailableResourceRootAbortsCatalogLoading)
{
    ResourceManager unavailable(std::filesystem::path{});
    EXPECT_DEATH(unavailable.LoadCatalog(), "");
}

TEST_F(ResourceManagerTest, EmptyCatalogReplacesRegistryAndReleasesManagerOwnership)
{
    WriteImage("pixel.tga", 0x11);
    Catalog({{"pixel", "pixel.tga"}});
    auto image = resources->GetImage("pixel"_sid);
    ASSERT_TRUE(image);
    Catalog({});
    EXPECT_EQ(resources->GetImage("pixel"_sid), nullptr);
    EXPECT_EQ(image->data[0], 0x11);
}

TEST_F(ResourceManagerTest, SingletonGettersUseThePreloadedCacheFromAnyWorkingDirectory)
{
    auto& manager = ResourceManager::Instance();
    std::filesystem::create_directories(manager.Root());
    auto const        catalogPath = manager.Root() / "resources.json";
    std::ifstream     previous(catalogPath);
    bool const        hadCatalog = previous.is_open();
    std::string const oldCatalog{std::istreambuf_iterator<char>(previous), std::istreambuf_iterator<char>()};
    previous.close();
    auto const relative = std::filesystem::path(directory.filename()) / "pixel.tga";
    struct Cleanup
    {
        ResourceManager&      manager;
        std::filesystem::path catalog;
        std::filesystem::path resources;
        std::filesystem::path workingDirectory;
        std::string           previousCatalog;
        bool                  hadCatalog;
        ~Cleanup()
        {
            manager.ClearCache();
            std::error_code error;
            std::filesystem::current_path(workingDirectory, error);
            std::filesystem::remove_all(resources, error);
            if (hadCatalog)
                std::ofstream(catalog) << previousCatalog;
            else
                std::filesystem::remove(catalog, error);
        }
    } cleanup{
        manager,
        catalogPath,
        manager.Root() / relative.parent_path(),
        std::filesystem::current_path(),
        oldCatalog,
        hadCatalog
    };
    WriteImage("pixel.tga", 0x55);
    std::filesystem::create_directory(cleanup.resources);
    std::filesystem::copy_file(directory / "Resources/pixel.tga", manager.Root() / relative);
    {
        std::ofstream file(catalogPath);
        file << Json::array({{{"id", "integration.pixel"}, {"path", relative.generic_string()}}}).dump();
    }
    manager.LoadCatalog();
    std::filesystem::remove(manager.Root() / relative);
    std::filesystem::current_path(directory);
    auto byId = manager.GetImage("integration.pixel"_sid);
    ASSERT_TRUE(byId);
    EXPECT_EQ(byId, manager.GetImage(polos::utils::StrHash64("integration.pixel")));
    EXPECT_EQ(byId, manager.GetImage("integration.pixel"_sid));
    EXPECT_EQ(byId->data[0], 0x55);
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
