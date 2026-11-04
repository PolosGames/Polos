#include "polos/resource/image.hpp"
#include "polos/resource/image/image_loader.hpp"
#include "polos/resource/storage.hpp"

#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <vector>

namespace
{
class ImageLoaderTest : public ::testing::Test
{
protected:
    std::filesystem::path directory;

    void SetUp() override
    {
        static std::atomic<unsigned> serial{};
        directory = std::filesystem::temp_directory_path() /
                    ("polos-image-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
                     "-" + std::to_string(serial++));
        std::filesystem::create_directory(directory);
    }

    void TearDown() override
    {
        std::filesystem::remove_all(directory);
    }
};

TEST_F(ImageLoaderTest, DecodesRgbaPixelsWithBottomRowFirst)
{
    // Two rows in a top-left-origin TGA, stored in BGRA byte order.
    std::array<unsigned char, 18> header{};
    header[2]  = 2;// Uncompressed true-color image.
    header[12] = 1;
    header[14] = 2;
    header[16] = 32;
    header[17] = 0x28;
    std::array<unsigned char, 4> const topPixel{0x33, 0x22, 0x11, 0xFF};
    std::array<unsigned char, 4> const bottomPixel{0x66, 0x55, 0x44, 0x80};
    std::array<unsigned char, 8> const expected{0x44, 0x55, 0x66, 0x80, 0x11, 0x22, 0x33, 0xFF};

    auto const path = directory / "pixel.tga";
    {
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<char const*>(header.data()), static_cast<std::streamsize>(header.size()));
        file.write(reinterpret_cast<char const*>(topPixel.data()), static_cast<std::streamsize>(topPixel.size()));
        file.write(reinterpret_cast<char const*>(bottomPixel.data()), static_cast<std::streamsize>(bottomPixel.size()));
    }

    auto const image = polos::resource::image::LoadImage(path);

    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->width, 1);
    EXPECT_EQ(image->height, 2);
    EXPECT_EQ(image->components, 4);
    ASSERT_EQ(image->data.size(), expected.size());
    for (std::size_t i{0U}; i < expected.size(); ++i)
    {
        EXPECT_EQ(image->data[i], expected[i]) << "RGBA byte " << i;
    }
}

TEST_F(ImageLoaderTest, ExpandsRgbAndGrayscaleInputsToCompleteRgbaBuffers)
{
    struct Fixture
    {
        char const*                  name;
        unsigned char                imageType;
        unsigned char                bitsPerPixel;
        unsigned char                descriptor;
        std::vector<unsigned char>   source;
        std::array<unsigned char, 8> expected;
    };

    std::array<Fixture, 3> const fixtures{
        Fixture{
            .name         = "rgb",
            .imageType    = 2,
            .bitsPerPixel = 24,
            .descriptor   = 0x20,
            .source       = {0x33, 0x22, 0x11, 0x66, 0x55, 0x44},
            .expected     = {0x11, 0x22, 0x33, 0xFF, 0x44, 0x55, 0x66, 0xFF},
        },
        Fixture{
            .name         = "gray",
            .imageType    = 3,
            .bitsPerPixel = 8,
            .descriptor   = 0x20,
            .source       = {0x22, 0x66},
            .expected     = {0x22, 0x22, 0x22, 0xFF, 0x66, 0x66, 0x66, 0xFF},
        },
        Fixture{
            .name         = "gray-alpha",
            .imageType    = 3,
            .bitsPerPixel = 16,
            .descriptor   = 0x28,
            .source       = {0x22, 0x80, 0x66, 0x40},
            .expected     = {0x22, 0x22, 0x22, 0x80, 0x66, 0x66, 0x66, 0x40},
        },
    };

    for (auto const& fixture : fixtures)
    {
        SCOPED_TRACE(fixture.name);
        // Two adjacent pixels expose both incorrect lengths and incorrect channel strides.
        std::array<unsigned char, 18> header{};
        header[2]       = fixture.imageType;
        header[12]      = 2;
        header[14]      = 1;
        header[16]      = fixture.bitsPerPixel;
        header[17]      = fixture.descriptor;
        auto const path = directory / (std::string(fixture.name) + ".tga");
        {
            std::ofstream file(path, std::ios::binary);
            file.write(reinterpret_cast<char const*>(header.data()), static_cast<std::streamsize>(header.size()));
            file.write(
                reinterpret_cast<char const*>(fixture.source.data()),
                static_cast<std::streamsize>(fixture.source.size()));
        }

        auto const image = polos::resource::image::LoadImage(path);
        ASSERT_NE(image, nullptr);
        EXPECT_EQ(image->width, 2);
        EXPECT_EQ(image->height, 1);
        EXPECT_EQ(image->components, 4);
        ASSERT_EQ(image->data.size(), fixture.expected.size());
        for (std::size_t i = 0; i < fixture.expected.size(); ++i)
        {
            EXPECT_EQ(image->data[i], fixture.expected[i]) << "byte " << i;
        }
    }
}

TEST_F(ImageLoaderTest, ReturnsNullForMissingOrMalformedImages)
{
    EXPECT_EQ(polos::resource::image::LoadImage(directory / "missing.png"), nullptr);

    auto const path = directory / "invalid.png";
    {
        std::ofstream file(path);
        file << "not an image";
    }

    EXPECT_EQ(polos::resource::image::LoadImage(path), nullptr);
}

TEST(ResourceStorageTest, ReportsInsertionSuccessAndDuplicateKeys)
{
    polos::resource::Storage<int> storage;
    auto                          resource = std::make_shared<int>(42);

    EXPECT_TRUE(storage.Add("resource"_sid, resource));

    EXPECT_FALSE(storage.Add("resource"_sid, resource));
    EXPECT_EQ(storage.Get("resource"_sid), resource);
    EXPECT_EQ(storage.Get("missing"_sid), nullptr);
}

TEST(ResourceStorageTest, RejectsNullAndDuplicateValuesWithoutReplacingExistingResources)
{
    polos::resource::Storage<int> storage;
    auto const                    original    = std::make_shared<int>(12);
    auto const                    replacement = std::make_shared<int>(99);
    ASSERT_TRUE(storage.Add("original"_sid, original));

    EXPECT_FALSE(storage.Add("missing"_sid, nullptr));
    EXPECT_FALSE(storage.Add("original"_sid, replacement));

    EXPECT_EQ(storage.Get("missing"_sid), nullptr);
    EXPECT_EQ(storage.Get("original"_sid), original);
    EXPECT_EQ(*storage.Get("original"_sid), 12);
}

TEST(ResourceStorageTest, ClearReleasesOwnershipWhileExternalReferencesSurvive)
{
    polos::resource::Storage<int> storage;
    auto                          owned    = std::make_shared<int>(24);
    std::weak_ptr<int> const      lifetime = owned;
    ASSERT_TRUE(storage.Add("owned"_sid, owned));
    auto external = storage.Get("owned"_sid);
    owned.reset();

    storage.Clear();
    storage.Clear();

    EXPECT_EQ(storage.Get("owned"_sid), nullptr);
    ASSERT_NE(external, nullptr);
    EXPECT_EQ(*external, 24);
    EXPECT_FALSE(lifetime.expired());

    external.reset();

    EXPECT_TRUE(lifetime.expired());
}

TEST(ResourceStorageTest, SwapExchangesCachesWithoutChangingResourceIdentity)
{
    polos::resource::Storage<int> first;
    polos::resource::Storage<int> second;
    auto const                    firstValue  = std::make_shared<int>(17);
    auto const                    secondValue = std::make_shared<int>(42);
    ASSERT_TRUE(first.Add("first"_sid, firstValue));
    ASSERT_TRUE(second.Add("second"_sid, secondValue));

    first.Swap(second);

    EXPECT_EQ(first.Get("first"_sid), nullptr);
    EXPECT_EQ(first.Get("second"_sid), secondValue);
    EXPECT_EQ(second.Get("first"_sid), firstValue);
    EXPECT_EQ(second.Get("second"_sid), nullptr);

    first.Swap(first);

    EXPECT_EQ(first.Get("second"_sid), secondValue);
}

}// namespace
