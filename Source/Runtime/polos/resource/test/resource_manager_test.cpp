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
#include <span>
#include <type_traits>
#include <vector>

namespace
{
using polos::resource::ResourceManager;
using Json = nlohmann::json;
static_assert(std::is_same_v<decltype(std::declval<ResourceManager&>().LoadCatalog()), void>);

// Accessors accept ResourceId only; strings and filesystem paths cannot select resources.
static_assert(!std::is_invocable_v<decltype(&ResourceManager::GetImage), ResourceManager const&, char const*>);
static_assert(!std::is_invocable_v<decltype(&ResourceManager::GetModel), ResourceManager const&, std::string_view>);
static_assert(!std::is_invocable_v<decltype(&ResourceManager::GetFile), ResourceManager const&, std::filesystem::path>);

struct ResourceCatalogCleanup
{
    ResourceManager&      manager;
    std::filesystem::path catalog;
    std::filesystem::path resources;
    std::filesystem::path workingDirectory;
    std::string           previousCatalog;
    bool                  hadCatalog;

    ~ResourceCatalogCleanup()
    {
        manager.ClearCache();
        std::error_code error;
        std::filesystem::current_path(workingDirectory, error);
        std::filesystem::remove_all(resources, error);
        if (hadCatalog)
        {
            std::ofstream(catalog) << previousCatalog;
        }
        else
        {
            std::filesystem::remove(catalog, error);
        }
    }
};

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

    auto Write(std::string_view path, std::string_view text) -> void
    {
        auto target = directory / "Resources" / path;
        std::filesystem::create_directories(target.parent_path());
        std::ofstream file(target);
        file << text;
    }

    auto Catalog(std::initializer_list<std::pair<std::string_view, std::string_view>> entries) -> void
    {
        auto catalog = Json::array();
        for (auto const& [id, path] : entries)
        {
            catalog.push_back({{"id", id}, {"path", path}});
        }
        Write("resources.json", catalog.dump());
        resources->LoadCatalog();
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
    EXPECT_EQ(resources->GetModel("model"_sid), nullptr);
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

TEST_F(ResourceManagerTest, LoadsEncodedImagesFromMemoryAndOwnsDecodedPixels)
{
    WriteImage("encoded.bin", 0x77);
    std::ifstream     input(directory / "Resources/encoded.bin", std::ios::binary);
    std::vector<char> encoded{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};

    auto image = resources->LoadImage("embedded.image"_sid, std::as_bytes(std::span(encoded)));
    encoded.assign(encoded.size(), '\0');

    ASSERT_NE(image, nullptr);
    EXPECT_EQ(resources->GetImage("embedded.image"_sid), image);
    EXPECT_EQ(image->width, 1);
    EXPECT_EQ(image->height, 1);
    EXPECT_EQ(image->components, 4);
    ASSERT_EQ(image->data.size(), 4U);
    EXPECT_EQ(image->data[0], 0x77);
    EXPECT_EQ(image->data[1], 0x22);
    EXPECT_EQ(image->data[2], 0x33);
    EXPECT_EQ(image->data[3], 0xFF);
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

TEST_F(ResourceManagerTest, LoadsOwnedFileBytesAndPreservesExternalReferencesAfterClearing)
{
    std::vector<std::byte> bytes{std::byte{0x42}, std::byte{0}, std::byte{0xFF}};
    auto const             expected = bytes;

    auto file = resources->LoadFile("embedded.file"_sid, std::span(bytes));
    bytes.assign(bytes.size(), std::byte{0});

    ASSERT_NE(file, nullptr);
    EXPECT_EQ(resources->GetFile("embedded.file"_sid), file);
    EXPECT_EQ(file->size, expected.size());
    EXPECT_EQ(file->uncompressedSize, expected.size());
    EXPECT_EQ(file->data, expected);
    EXPECT_TRUE(file->path.empty());
    EXPECT_TRUE(file->stemName.empty());

    resources->ClearCache();

    EXPECT_EQ(resources->GetFile("embedded.file"_sid), nullptr);
    EXPECT_EQ(file->data, expected);
}

TEST_F(ResourceManagerTest, LoadsEmptyFilesFromPathsAndMemory)
{
    Write("empty.bin", "");
    auto fromPath = resources->LoadFile("empty.path"_sid, directory / "Resources/empty.bin");
    auto fromData = resources->LoadFile("empty.data"_sid, std::span<std::byte const>{});

    for (auto const& file : {fromPath, fromData})
    {
        ASSERT_NE(file, nullptr);
        EXPECT_TRUE(file->data.empty());
        EXPECT_EQ(file->size, 0U);
        EXPECT_EQ(file->uncompressedSize, 0U);
    }
}

TEST_F(ResourceManagerTest, ReusesCachedRepresentationsAcrossLoadOverloads)
{
    WriteImage("pixel.tga", 0x11);
    auto const imagePath = directory / "Resources/pixel.tga";
    auto       image     = resources->LoadImage("cached.image"_sid, imagePath);
    std::filesystem::remove(imagePath);
    std::array<std::byte, 1> const bytes{std::byte{0x42}};

    ASSERT_NE(image, nullptr);
    EXPECT_EQ(resources->LoadImage("cached.image"_sid, std::span(bytes)), image);
    EXPECT_EQ(resources->LoadImage("cached.image"_sid, imagePath), image);

    auto file = resources->LoadFile("cached.file"_sid, std::span(bytes));
    ASSERT_NE(file, nullptr);
    EXPECT_EQ(resources->LoadFile("cached.file"_sid, directory / "missing.bin"), file);
    EXPECT_EQ(resources->LoadFile("cached.file"_sid, std::span<std::byte const>{}), file);
}

TEST_F(ResourceManagerTest, FailedExplicitLoadsLeaveIdsAvailableForRetry)
{
    auto const                     missing = directory / "Resources/pixel.tga";
    std::array<std::byte, 3> const invalid{std::byte{0x42}, std::byte{0}, std::byte{0xFF}};

    EXPECT_EQ(resources->LoadImage("retry.image"_sid, missing), nullptr);
    EXPECT_EQ(resources->LoadImage("retry.image"_sid, std::span(invalid)), nullptr);
    EXPECT_EQ(resources->LoadImage("retry.image"_sid, std::span<std::byte const>{}), nullptr);
    EXPECT_EQ(resources->GetImage("retry.image"_sid), nullptr);
    EXPECT_EQ(resources->LoadFile("retry.file"_sid, missing), nullptr);
    EXPECT_EQ(resources->GetFile("retry.file"_sid), nullptr);

    WriteImage("pixel.tga", 0x55);
    auto image = resources->LoadImage("retry.image"_sid, missing);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->data[0], 0x55);
    EXPECT_NE(resources->LoadFile("retry.file"_sid, std::span(invalid)), nullptr);
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

    for (auto const id : {std::string_view{}, std::string_view("a\0b", 3)})
    {
        EXPECT_DEATH(Catalog({{id, "a.bin"}}), "");
    }
}

TEST_F(ResourceManagerTest, EscapingSymlinksAbortCatalogLoading)
{
    Write("safe/data.txt", "inside");
    std::filesystem::create_directory(directory / "outside");
    std::error_code error;
    std::filesystem::create_directory_symlink(directory / "outside", directory / "Resources/link", error);
    if (error)
    {
        GTEST_SKIP() << "Symlinks unavailable: " << error.message();
    }

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
        SCOPED_TRACE(catalog);
        Write("resources.json", catalog);
        EXPECT_DEATH(resources->LoadCatalog(), "");
    }
}

TEST_F(ResourceManagerTest, MissingOrMalformedResourcesAbortImmediately)
{
    for (auto missing : {"missing.tga", "missing.gltf", "missing.spv"})
    {
        SCOPED_TRACE(missing);
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
    auto& manager = ResourceManager::Get();
    std::filesystem::create_directories(manager.Root());
    auto const        catalogPath = manager.Root() / "resources.json";
    std::ifstream     previous(catalogPath);
    bool const        hadCatalog = previous.is_open();
    std::string const oldCatalog{std::istreambuf_iterator<char>(previous), std::istreambuf_iterator<char>()};
    previous.close();
    auto const             relative = std::filesystem::path(directory.filename()) / "pixel.tga";
    ResourceCatalogCleanup cleanup{
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
